#include <journal/file_journal.hpp>

namespace journal
{
    FileJournal::FileJournal(const std::filesystem::path& filePath, const LogLevel logLevel)
    : Journal(logLevel)
    {
        if (filePath.empty())
        {
            journal_status_ = JournalStatus::InvalidArgument;
            return;
        }

        journal_.open(filePath, std::ios::app);

        if (!is_ready())
        {
            journal_status_ = JournalStatus::FileOpenFailed;
        }
    }

    bool FileJournal::is_ready() const
    {
        return journal_.is_open();
    }

    JournalStatus FileJournal::transport(const std::string_view logMsg)
    {
        journal_ << logMsg;
        journal_.flush();

        if (!journal_.good())
        {
            return JournalStatus::FileWriteFailed;
        }

        return JournalStatus::Success;
    }

    JournalStatus FileJournal::not_ready_status() const noexcept
    {
        return JournalStatus::FileOpenFailed;
    }
}
