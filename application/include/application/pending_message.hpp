#pragma once

#include <journal/log_level.hpp>

#include <string>
#include <chrono>
#include <optional>

namespace application
{
    struct PendingMessage
    {
        std::string message;
        std::chrono::system_clock::time_point timestamp;
        std::optional<journal::LogLevel> level;
    };
}