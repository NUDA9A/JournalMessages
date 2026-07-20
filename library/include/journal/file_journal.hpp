#pragma once

#include <journal/journal_status.hpp>
#include <journal/log_level.hpp>

#include <fstream>
#include <string_view>
#include <filesystem>
#include <chrono>

namespace journal
{
    class FileJournal
    {
    public:
        FileJournal() = delete;
        FileJournal(const FileJournal&) = delete;
        FileJournal& operator=(const FileJournal&) = delete;
        FileJournal(FileJournal&&) noexcept = delete;
        FileJournal& operator=(FileJournal&&) noexcept = delete;

        ~FileJournal() = default;

        FileJournal(const std::filesystem::path& filePath, LogLevel logLevel);

        bool is_open() const;
        JournalStatus status() const noexcept;
        void write(std::string_view message, const std::chrono::system_clock::time_point& timePoint, LogLevel logLevel);
        void write(std::string_view message, const std::chrono::system_clock::time_point& timePoint);
        void setLogLevel(LogLevel logLevel) noexcept;
    private:
        std::ofstream journal_{};
        LogLevel log_level_;
        JournalStatus journal_status_;
    };
}