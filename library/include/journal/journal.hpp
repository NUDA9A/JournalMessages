#pragma once

#include <journal/log_level.hpp>
#include <journal/journal_status.hpp>

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>
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

            const auto logMsg = form_log_message(message, timePoint, logLevel);
            if (logMsg.empty())
            {
                return;
            }
            journal_status_ = transport(logMsg);
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

        virtual JournalStatus transport(std::string_view logMsg) = 0;
        virtual JournalStatus not_ready_status() const noexcept = 0;

    private:
        std::string form_log_message(const std::string_view message, const std::chrono::system_clock::time_point& timePoint, const LogLevel logLevel)
        {
            const auto currentTime = std::chrono::system_clock::to_time_t(timePoint);
            const std::tm* currentDateTime = std::localtime(&currentTime);

            if (currentDateTime == nullptr)
            {
                journal_status_ = JournalStatus::TimeConversionFailed;
                return "";
            }

            std::ostringstream oss;

            oss << std::put_time(currentDateTime, "[%Y-%m-%d %H:%M:%S]");
            oss << " [" << to_string(logLevel) << "] " << message << "\n";

            return oss.str();
        }

        LogLevel log_level_;

    protected:
        JournalStatus journal_status_;
    };
}