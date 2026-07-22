#pragma once

#include <journal/log_level.hpp>
#include <journal/journal_status.hpp>
#include <journal/log_record.hpp>

#include <chrono>
#include <string_view>

namespace journal
{
    class Journal
    {
    public:
        Journal(const Journal&) = delete;
        Journal& operator=(const Journal&) = delete;
        Journal(Journal&&) = delete;
        Journal& operator=(Journal&&) = delete;

        void write(const std::string_view message, const std::chrono::system_clock::time_point& timePoint, const LogLevel logLevel)
        {
            if (!is_ready())
            {
                journal_status_ = not_ready_status();
                return;
            }

            if (!is_compatible(logLevel, log_level_))
            {
                journal_status_ = JournalStatus::FilteredOut;
                return;
            }

            const LogRecordView record{message, logLevel, timePoint};

            journal_status_ = transport(record);
        }

        void write(const std::string_view message, const std::chrono::system_clock::time_point& timePoint)
        {
            return write(message, timePoint, log_level_);
        }

        JournalStatus status() const noexcept
        {
            return journal_status_;
        }

        void setLogLevel(const LogLevel logLevel) noexcept
        {
            log_level_ = logLevel;
        }

        virtual ~Journal() = default;

        virtual bool is_ready() const = 0;

    protected:
        explicit Journal(const LogLevel logLevel) : log_level_(logLevel), journal_status_(JournalStatus::Success) {}

        virtual JournalStatus transport(const LogRecordView& logRecord) = 0;
        virtual JournalStatus not_ready_status() const noexcept = 0;

    private:
        LogLevel log_level_;

    protected:
        JournalStatus journal_status_;
    };
}