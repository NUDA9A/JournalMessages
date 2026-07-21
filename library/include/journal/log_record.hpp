#pragma once

#include <string_view>
#include <chrono>

#include <journal/log_level.hpp>

namespace journal
{
    struct LogRecordView
    {
        std::string_view message;
        LogLevel level;
        std::chrono::system_clock::time_point timestamp;
    };
}