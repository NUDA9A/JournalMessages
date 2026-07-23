#pragma once

#include <collector/collector_status.hpp>
#include <collector/received_log_record.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <optional>


namespace collector
{
    class TCPServer
    {
    public:
        static constexpr std::size_t MAX_MESSAGE_SIZE = 1024;

        TCPServer(const std::string& ipAddress, std::uint16_t port);

        TCPServer() = delete;
        TCPServer(const TCPServer&) = delete;
        TCPServer(TCPServer&&) = delete;
        TCPServer& operator=(const TCPServer&) = delete;
        TCPServer& operator=(TCPServer&&) = delete;

        ~TCPServer();

        void acceptClient() noexcept;
        std::optional<ReceivedLogRecord> receive();
        CollectorStatus status() const noexcept;
    private:
        CollectorStatus readAll(std::byte* buf_ptr, std::size_t size) noexcept;

        void closeListening(CollectorStatus status) noexcept;
        void closeClient() noexcept;

        int listening_fd_ = -1;
        int client_fd_ = -1;
        CollectorStatus status_ = CollectorStatus::Success;
    };
}
