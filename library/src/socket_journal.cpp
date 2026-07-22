#include <journal/socket_journal.hpp>

#include <journal/socket_protocol.hpp>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cerrno>

namespace journal
{
    SocketJournal::SocketJournal(const std::string& ipAddress, const std::uint16_t port, const LogLevel logLevel) : Journal(logLevel)
    {
        if (ipAddress.empty() || port == 0)
        {
            journal_status_ = JournalStatus::InvalidArgument;
            return;
        }

        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0)
        {
            journal_status_ = JournalStatus::SocketCreateFailed;
            return;
        }

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        int res = inet_pton(AF_INET, ipAddress.c_str(), &address.sin_addr);
        if (res < 1)
        {
            journal_status_ = JournalStatus::InvalidAddress;
            close_socket();
            return;
        }

        res = connect(fd_, reinterpret_cast<sockaddr*>(&address), sizeof(address));

        if (res < 0)
        {
            journal_status_ = JournalStatus::ConnectionFailed;
            close_socket();
            return;
        }
    }

    void SocketJournal::close_socket() noexcept
    {
        if (fd_ >= 0)
        {
            close(fd_);
        }
        fd_ = -1;
    }

    SocketJournal::~SocketJournal()
    {
        close_socket();
    }

    bool SocketJournal::is_ready() const
    {
        return fd_ >= 0;
    }

    JournalStatus SocketJournal::not_ready_status() const noexcept
    {
        return JournalStatus::ConnectionFailed;
    }

    JournalStatus SocketJournal::transport(const LogRecordView& logRecord)
    {
        std::vector<std::byte> data;
        const auto res = serialize(logRecord, data);
        if (res != JournalStatus::Success)
        {
            return res;
        }

        return send_all(data);
    }

    JournalStatus SocketJournal::send_all(const std::vector<std::byte>& data)
    {
        std::size_t sentBytes = 0;
        while (sentBytes < data.size())
        {
            const auto res = send(fd_, data.data() + sentBytes, data.size() - sentBytes, MSG_NOSIGNAL);
            if (res > 0)
            {
                sentBytes += res;
            } else if (res == -1 && errno == EINTR)
            {
                continue;
            } else
            {
                close_socket();
                return JournalStatus::SocketSendFailed;
            }
        }

        return JournalStatus::Success;
    }
}
