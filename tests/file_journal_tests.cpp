#include "tests_suites.hpp"

#include <filesystem>
#include <cstddef>
#include <string>
#include <utility>
#include <system_error>
#include <optional>
#include <fstream>
#include <iterator>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string_view>

#include <journal/file_journal.hpp>

namespace {
    std::size_t TEST_UNIQUE_NUMBER = 0;

    class TempPath
    {
    public:
        TempPath() = delete;
        TempPath(const TempPath&) = delete;
        TempPath& operator=(const TempPath&) = delete;
        TempPath(TempPath&&) = delete;
        TempPath& operator=(TempPath&&) = delete;

        explicit TempPath(std::filesystem::path tmpPath) : path_(std::move(tmpPath))
        {
            const std::string tmpFileName = "journal_test_" + std::to_string(TEST_UNIQUE_NUMBER++) + ".log";

            path_.append(tmpFileName);

            deleteTmpFile();
        }

        ~TempPath()
        {
            deleteTmpFile();
        }

        const std::filesystem::path& path() const noexcept
        {
            return path_;
        }
    private:
        void deleteTmpFile() const noexcept
        {
            std::error_code ec{};
            std::filesystem::remove(path_, ec);
        }

        std::filesystem::path path_;
    };

    std::optional<std::string> read_file_contents(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            return std::nullopt;
        }

        std::string res{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
        if (file.bad())
        {
            return std::nullopt;
        }

        return res;
    }
    
    std::string form_expected_file_contents(const std::chrono::system_clock::time_point& time, const std::string_view message, const journal::LogLevel logLevel)
    {
        std::ostringstream required_log;

        const auto currentTime = std::chrono::system_clock::to_time_t(time);
        const std::tm* currentDateTime = std::localtime(&currentTime);

        if (currentDateTime == nullptr)
        {
            return "";
        }

        required_log << std::put_time(currentDateTime, "[%Y-%m-%d %H:%M:%S]");
        required_log << " [" << to_string(logLevel) << "] " << message << "\n";
        
        return required_log.str();
    }
}

namespace journal::tests
{
    static void file_journal_init_test(TestContext& testContext)
    {
        const TempPath tmpPath{std::filesystem::temp_directory_path()};
        const FileJournal journal(tmpPath.path(), LogLevel::Info);

        testContext.check(journal.is_open(), "FileJournal initialization test: Can not open file");
        testContext.check(journal.status() == JournalStatus::Success, "FileJournal initialization test: journal.status() != JournalStatus::Success after initialization");
    }

    static void file_journal_invalid_path_test(TestContext& testContext)
    {
        const FileJournal journalEmptyPath("", LogLevel::Info);
        const FileJournal journalNonExistingPath("non/existing/path.log", LogLevel::Info);

        testContext.check(!journalEmptyPath.is_open(), "FileJournal invalid path test: FileJournal opened file with empty path");
        testContext.check(journalEmptyPath.status() == JournalStatus::InvalidArgument, "FileJournal invalid path test: journal.status() != JournalStatus::InvalidArgument for empty path");
        testContext.check(!journalNonExistingPath.is_open(), "FileJournal invalid path test: FileJournal opened non existing file");
        testContext.check(journalNonExistingPath.status() == JournalStatus::FileOpenFailed, "FileJournal invalid path test: journal.status() != JournalStatus::FileOpenFailed for non existsing path");
    }

    static void file_journal_write_test(TestContext& testContext)
    {
        const TempPath tmpPath{std::filesystem::temp_directory_path()};
        const std::chrono::system_clock::time_point time{std::chrono::system_clock::now()};

        {
            FileJournal journal{tmpPath.path(), LogLevel::Info};

            journal.write("Test message", time, LogLevel::Warning);
            testContext.check(journal.status() == JournalStatus::Success, "FileJournal::write() test: journal.status() != JournalStatus::Success after write");
        }

        const auto fileContents = read_file_contents(tmpPath.path());
        const bool hasResult = fileContents.has_value();
        testContext.check(hasResult, "FileJournal::write() test: Can not read log file");

        if (!hasResult)
        {
            return;
        }

        testContext.check(*fileContents == form_expected_file_contents(time, "Test message", LogLevel::Warning), "FileJournal::write() test: log message does not equal to required");
    }

    static void file_journal_filtering_test(TestContext& testContext)
    {
        const TempPath tmpPath{std::filesystem::temp_directory_path()};

        const std::chrono::system_clock::time_point time{std::chrono::system_clock::now()};

        {
            FileJournal journalWarning(tmpPath.path(), LogLevel::Warning);

            journalWarning.write("Test message", time, LogLevel::Info);
            testContext.check(journalWarning.status() == JournalStatus::FilteredOut, "FileJournal filtering test: journal.status() != JournalStatus::FilteredOut after writing LogLevel::Info message for LogLevel::Warning journal");
            
            const auto fileContents = read_file_contents(tmpPath.path());
            const bool hasValue = fileContents.has_value();
            testContext.check(hasValue, "FileJournal filtering test: can not read file");

            if (hasValue)
            {
                testContext.check(fileContents->empty(), "FileJournal filtering test: file is not empty after writing LogLevel::Info message into LogLevel::Warning journal");
            }

            journalWarning.write("Test message", time);
            testContext.check(journalWarning.status() == JournalStatus::Success, "FileJournal filtering test: journal.status() != JournalStatus::Success after correct write()");
            journalWarning.write("Test message2", time, LogLevel::Warning);
            testContext.check(journalWarning.status() == JournalStatus::Success, "FileJournal filtering test: journal.status() != JournalStatus::Success after correct write()");
            journalWarning.write("Test message3", time, LogLevel::Error);
            testContext.check(journalWarning.status() == JournalStatus::Success, "FileJournal filtering test: journal.status() != JournalStatus::Success after correct write()");
        }
        
        const auto fileContents = read_file_contents(tmpPath.path());
        const bool hasResult = fileContents.has_value();
        testContext.check(hasResult, "FileJournal::write() test: Can not read log file");

        if (!hasResult)
        {
            return;
        }
        
        const std::string requiredLog = 
            form_expected_file_contents(time, "Test message", LogLevel::Warning) + 
                form_expected_file_contents(time, "Test message2", LogLevel::Warning) + 
                    form_expected_file_contents(time, "Test message3", LogLevel::Error);
        
        testContext.check(*fileContents == requiredLog, "FileJournal filtering test: actual log != expected log");
    }
    
    static void file_journal_set_log_level_test(TestContext& testContext)
    {
        const TempPath tmpPath{std::filesystem::temp_directory_path()};
        
        const std::chrono::system_clock::time_point time{std::chrono::system_clock::now()};
        
        {
            FileJournal journal(tmpPath.path(), LogLevel::Info);
            
            journal.write("Test message", time);
            testContext.check(journal.status() == JournalStatus::Success, "FileJournal::set_log_level test: journal.staus() != JournalStatus::Success after correct write");
            journal.setLogLevel(LogLevel::Error);
            journal.write("Test message2", time, LogLevel::Warning);
            testContext.check(journal.status() == JournalStatus::FilteredOut, "FileJournal::set_log_level test: journal.staus() != JournalStatus::FilteredOut after LogLevel::Warning write into LogLevel::Error journal");
            journal.write("Test message3", time);
            testContext.check(journal.status() == JournalStatus::Success, "FileJournal::set_log_level test: journal.staus() != JournalStatus::Success after correct write");
        }
        
        const auto fileContents = read_file_contents(tmpPath.path());
        const bool hasResult = fileContents.has_value();
        testContext.check(hasResult, "FileJournal::set_log_level() test: Can not read log file");

        if (!hasResult)
        {
            return;
        }
        
        const std::string requiredLog = 
            form_expected_file_contents(time, "Test message", LogLevel::Info) + 
                form_expected_file_contents(time, "Test message3", LogLevel::Error);
        
        testContext.check(*fileContents == requiredLog, "FileJournal::set_log_level() test: actual log != expected log");
    }
    
    static void file_journal_append_test(TestContext& testContext)
    {
        const TempPath tmpPath{std::filesystem::temp_directory_path()};
        
        const std::chrono::system_clock::time_point time{std::chrono::system_clock::now()};
        
        {
            FileJournal journal(tmpPath.path(), LogLevel::Info);
            
            journal.write("First", time);
        }
        
        {
            FileJournal journal(tmpPath.path(), LogLevel::Info);
            
            journal.write("Second", time);
        }
        
        const auto fileContents = read_file_contents(tmpPath.path());
        const bool hasResult = fileContents.has_value();
        testContext.check(hasResult, "FileJournal append test: Can not read log file");

        if (!hasResult)
        {
            return;
        }
        
        const std::string requiredLog = 
            form_expected_file_contents(time, "First", LogLevel::Info) + 
                form_expected_file_contents(time, "Second", LogLevel::Info);
        
        testContext.check(*fileContents == requiredLog, "FileJournal append test: actual log != expected log");
    }

    void run_file_journal_tests(TestContext& testContext)
    {
        file_journal_init_test(testContext);
        file_journal_invalid_path_test(testContext);
        file_journal_write_test(testContext);
        file_journal_filtering_test(testContext);
        file_journal_set_log_level_test(testContext);
        file_journal_append_test(testContext);
    }
}
