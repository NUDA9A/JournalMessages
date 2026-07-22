#pragma once

#include <journal/journal.hpp>

#include <string>
#include <cstdint>
#include <cstddef>
#include <vector>

namespace journal
{
    class SocketJournal final : public Journal
    {
    public:
        SocketJournal() = delete;
        SocketJournal(const SocketJournal&) = delete;
        SocketJournal(SocketJournal&&) = delete;
        SocketJournal& operator=(const SocketJournal&) = delete;
        SocketJournal& operator=(SocketJournal&&) = delete;

        ~SocketJournal() override;

        SocketJournal(const std::string&, std::uint16_t, LogLevel);

        bool is_ready() const override;
    private:
        JournalStatus transport(const LogRecordView& logRecord) override;
        JournalStatus not_ready_status() const noexcept override;
        JournalStatus send_all(const std::vector<std::byte>&);

        void close_socket() noexcept;

        int fd_ = -1;
    };
}