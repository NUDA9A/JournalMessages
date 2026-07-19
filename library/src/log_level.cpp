#include <journal/log_level.hpp>

#include <exception>

namespace journal
{
    std::string to_string(const LogLevel level) noexcept
    {
        switch (level)
        {
        case LogLevel::Info:
            return "Info";
        case LogLevel::Warning:
            return "Warning";
        case LogLevel::Error:
            return "Error";
        default:
            return "UnknownLogLevel";
        }
    }

    std::optional<LogLevel> to_log_level(const std::string& level) noexcept
    {
        if (level == "Info" || level == "INFO" || level == "info")
        {
            return LogLevel::Info;
        }

        if (level == "Warning" || level == "WARNING" || level == "warning")
        {
            return LogLevel::Warning;
        }

        if (level == "Error" || level == "ERROR" || level == "error")
        {
            return LogLevel::Error;
        }

        return std::nullopt;
    }
}