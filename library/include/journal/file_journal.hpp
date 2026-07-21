#pragma once

#include <journal/journal_status.hpp>
#include <journal/log_level.hpp>
#include <journal/journal.hpp>

#include <fstream>
#include <string_view>
#include <filesystem>

namespace journal
{
    class FileJournal : public Journal
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
        JournalStatus transport(std::string_view logMsg) override;
        JournalStatus not_ready_status() const noexcept override;

        std::ofstream journal_{};
    };
}