#include <journal/log_level.hpp>

#include <cctype>
#include <cstddef>

static bool equals_ignore_case(const std::string_view lhs, const std::string_view rhs) noexcept
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }

    for (std::size_t i = 0; i < lhs.size(); ++i)
    {
        if (std::tolower(static_cast<unsigned char>(lhs[i])) != std::tolower(static_cast<unsigned char>(rhs[i])))
        {
            return false;
        }
    }

    return true;
}

namespace journal
{
    std::string_view to_string(const LogLevel level) noexcept
    {
        switch (level)
        {
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        default:
            return "UNKNOWN_LOG_LEVEL";
        }
    }

    std::optional<LogLevel> to_log_level(const std::string_view level) noexcept
    {
        if (equals_ignore_case(level, "info"))
        {
            return LogLevel::Info;
        }

        if (equals_ignore_case(level, "warning"))
        {
            return LogLevel::Warning;
        }

        if (equals_ignore_case(level, "error"))
        {
            return LogLevel::Error;
        }

        return std::nullopt;
    }
}