#pragma once

#include <journal/journal_status.hpp>
#include <journal/log_level.hpp>
#include <journal/journal.hpp>
#include <journal/log_record.hpp>

#include <fstream>
#include <filesystem>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>

namespace journal
{
    class FileJournal final : public Journal
    {
    public:
        FileJournal() = delete;
        FileJournal(const FileJournal&) = delete;
        FileJournal& operator=(const FileJournal&) = delete;
        FileJournal(FileJournal&&) noexcept = delete;
        FileJournal& operator=(FileJournal&&) noexcept = delete;

        ~FileJournal() override = default;

        FileJournal(const std::filesystem::path& filePath, LogLevel logLevel);

        bool is_ready() const override;
    private:
        std::string form_log_message(const LogRecordView& logRecord)
        {
            const auto currentTime = std::chrono::system_clock::to_time_t(logRecord.timestamp);
            const std::tm* currentDateTime = std::localtime(&currentTime);

            if (currentDateTime == nullptr)
            {
                journal_status_ = JournalStatus::TimeConversionFailed;
                return "";
            }

            std::ostringstream oss;

            oss << std::put_time(currentDateTime, "[%Y-%m-%d %H:%M:%S]");
            oss << " [" << to_string(logRecord.level) << "] " << logRecord.message << "\n";

            return oss.str();
        }

        JournalStatus transport(const LogRecordView&) override;
        JournalStatus not_ready_status() const noexcept override;

        std::ofstream journal_{};
    };
}