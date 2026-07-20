#pragma once

#include <string_view>
#include <optional>

namespace journal
{
    enum class LogLevel
    {
        Info,
        Warning,
        Error
    };

    std::string_view to_string(LogLevel level) noexcept;
    std::optional<LogLevel> to_log_level(std::string_view level) noexcept;
    bool is_compatible(LogLevel msgLevel, LogLevel requiredLevel) noexcept;
}