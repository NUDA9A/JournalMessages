#pragma once

#include <journal/log_level.hpp>

#include <string>
#include <chrono>

namespace collector
{
    struct ReceivedLogRecord
    {
        std::string message;
        journal::LogLevel level;
        std::chrono::system_clock::time_point timestamp;
    };
}