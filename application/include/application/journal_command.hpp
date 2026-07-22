#pragma once

#include <application/pending_message.hpp>
#include <journal/log_level.hpp>

#include <variant>

namespace application
{
    struct JournalCommand
    {
        std::variant<PendingMessage, journal::LogLevel> command;
    };
}