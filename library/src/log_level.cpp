#include <journal/log_level.hpp>

#include <string>
#include <algorithm>

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

    std::string to_lower_string(const std::string_view sv)
    {
        std::string res{sv};

        std::transform(res.begin(), res.end(), res.begin(), ::tolower);

        return res;
    }

    std::optional<LogLevel> to_log_level(const std::string_view level) noexcept
    {
        auto loweredLevel = to_lower_string(level);

        if (loweredLevel == "info")
        {
            return LogLevel::Info;
        }

        if (loweredLevel == "warning")
        {
            return LogLevel::Warning;
        }

        if (loweredLevel == "error")
        {
            return LogLevel::Error;
        }

        return std::nullopt;
    }
}