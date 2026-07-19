#pragma once

#include <string>
#include <optional>

namespace journal
{
    enum class LogLevel
    {
        Info,
        Warning,
        Error
    };

    std::string to_string(LogLevel level) noexcept;
    std::optional<LogLevel> to_log_level(const std::string& level) noexcept;
}