#include <collector/tcp_server.hpp>

#include <journal/log_level.hpp>
#include <journal/socket_protocol.hpp>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cerrno>
#include <chrono>
#include <optional>
#include <cstdint>
#include <string>
#include <limits>
#include <utility>


namespace collector
{
    TCPServer::TCPServer(const std::string& ipAddress, std::uint16_t port)
    {
        listening_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listening_fd_ == -1)
        {
            status_ = CollectorStatus::SocketCreateFailed;
            return;
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (inet_pton(AF_INET, ipAddress.c_str(), &addr.sin_addr) < 1)
        {
            closeListening(CollectorStatus::InvalidIpAddress);
            return;
        }

        int opt = 1;
        if (setsockopt(listening_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            closeListening(CollectorStatus::SetReuseAddrFailed);
            return;
        }

        if (bind(listening_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        {
            closeListening(CollectorStatus::BindFailed);
            return;
        }

        if (listen(listening_fd_, 1) < 0)
        {
            closeListening(CollectorStatus::ListenFailed);
            return;
        }

        status_ = CollectorStatus::Success;
    }

    TCPServer::~TCPServer()
    {
        if (client_fd_ >= 0)
        {
            closeClient();
        }
        if (listening_fd_ >= 0)
        {
            closeListening(status_);
        }
    }

    void TCPServer::closeListening(const CollectorStatus status) noexcept
    {
        status_ = status;
        close(listening_fd_);
        listening_fd_ = -1;
    }

    void TCPServer::closeClient() noexcept
    {
        close(client_fd_);
        client_fd_ = -1;
    }

    void TCPServer::acceptClient() noexcept
    {
        if (listening_fd_ < 0)
        {
            return;
        }

        if (client_fd_ >= 0)
        {
            closeClient();
        }

        while (true)
        {
            client_fd_ = accept(listening_fd_, nullptr, nullptr);
            if (client_fd_ == -1)
            {
                if (errno == EINTR)
                {
                    continue;
                }

                status_ = CollectorStatus::AcceptFailed;
                client_fd_ = -1;
                return;
            }

            break;
        }

        status_ = CollectorStatus::Success;
    }

    CollectorStatus TCPServer::readAll(std::byte* buf_ptr, std::size_t size) noexcept
    {
        if (size == 0)
        {
            return CollectorStatus::Success;
        }

        std::size_t read_size = 0;
        while (read_size < size)
        {
            const auto res = recv(client_fd_, buf_ptr + read_size, size - read_size, 0);
            if (res > 0)
            {
                read_size += res;
            } else if (res == -1 && errno != EINTR)
            {
                closeClient();
                return CollectorStatus::ReadFailed;
            } else if (res == 0)
            {
                closeClient();
                return CollectorStatus::ClientDisconnected;
            }
        }

        return CollectorStatus::Success;
    }

    std::optional<ReceivedLogRecord> TCPServer::receive()
    {
        std::byte buf[journal::HEADER_SIZE];
        if (status_ = readAll(buf, journal::HEADER_SIZE); status_ != CollectorStatus::Success)
        {
            return std::nullopt;
        }

        std::uint32_t message_size = 0;

        for (std::size_t i = 0; i < 4; ++i)
        {
            message_size |= std::to_integer<std::uint32_t>(buf[i]) << (4 - i - 1) * 8;
        }

        if (message_size > MAX_MESSAGE_SIZE)
        {
            status_ = CollectorStatus::MessageTooLarge;
            closeClient();
            return std::nullopt;
        }

        if (std::to_integer<std::uint8_t>(buf[4]) >= static_cast<std::uint8_t>(journal::LogLevel::Size))
        {
            status_ = CollectorStatus::BadPacket;
            closeClient();
            return std::nullopt;
        }

        auto level = static_cast<journal::LogLevel>(std::to_integer<std::uint8_t>(buf[4]));

        std::uint64_t timestamp_ms = 0;

        for (std::size_t i = 0; i < 8; ++i)
        {
            timestamp_ms |= std::to_integer<std::uint64_t>(buf[i + 5]) << (8 - i - 1) * 8;
        }

        if (static_cast<std::uint64_t>(std::numeric_limits<std::chrono::milliseconds::rep>::max()) < timestamp_ms)
        {
            status_ = CollectorStatus::BadPacket;
            closeClient();
            return std::nullopt;
        }

        std::chrono::system_clock::time_point timestamp{
            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                std::chrono::milliseconds{
                    static_cast<std::chrono::milliseconds::rep>(timestamp_ms)
                }
            )
        };

        std::string message;
        message.resize(message_size);

        if (status_ = readAll(reinterpret_cast<std::byte*>(message.data()), message.size()); status_ != CollectorStatus::Success)
        {
            return std::nullopt;
        }

        return ReceivedLogRecord{std::move(message), level, timestamp};
    }

    CollectorStatus TCPServer::status() const noexcept
    {
        return status_;
    }
}
