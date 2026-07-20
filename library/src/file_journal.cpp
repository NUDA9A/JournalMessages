#include <journal/file_journal.hpp>

#include <ctime>
#include <iomanip>
#include <sstream>

namespace journal
{
    FileJournal::FileJournal(const std::filesystem::path& filePath, const LogLevel logLevel)
    : log_level_(logLevel),
    journal_status_(JournalStatus::Success)
    {
        if (filePath.empty())
        {
            journal_status_ = JournalStatus::InvalidArgument;
            return;
        }

        journal_.open(filePath, std::ios::app);

        if (!is_open())
        {
            journal_status_ = JournalStatus::FileOpenFailed;
        }
    }

    bool FileJournal::is_open() const
    {
        return journal_.is_open();
    }

    JournalStatus FileJournal::status() const noexcept
    {
        return journal_status_;
    }

    void FileJournal::write(const std::string_view message, const std::chrono::system_clock::time_point& timePoint, const LogLevel logLevel)
    {
        if (!is_open())
        {
            journal_status_ = JournalStatus::FileOpenFailed;
            return;
        }

        if (!is_compatible(logLevel, log_level_))
        {
            journal_status_ = JournalStatus::FilteredOut;
            return;
        }

        const auto currentTime = std::chrono::system_clock::to_time_t(timePoint);
        const std::tm* currentDateTime = std::localtime(&currentTime);

        if (currentDateTime == nullptr)
        {
            journal_status_ = JournalStatus::TimeConversionFailed;
            return;
        }

        std::ostringstream oss;

        oss << std::put_time(currentDateTime, "[%Y-%m-%d %H:%M:%S]");
        oss << " [" << to_string(logLevel) << "] " << message << "\n";

        const auto logMsg = oss.str();

        journal_ << logMsg;
        journal_.flush();

        if (!journal_.good())
        {
            journal_status_ = JournalStatus::FileWriteFailed;
            return;
        }

        journal_status_ = JournalStatus::Success;
    }

    void FileJournal::write(const std::string_view message, const std::chrono::system_clock::time_point& timePoint)
    {
        return write(message, timePoint, log_level_);
    }

    void FileJournal::setLogLevel(const LogLevel logLevel) noexcept
    {
        log_level_ = logLevel;
    }
}
