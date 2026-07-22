#pragma once

#include <string_view>

namespace journal
{
    enum class JournalStatus
    {
        Success,
        FilteredOut,
        InvalidArgument,
        FileOpenFailed,
        FileWriteFailed,
        TimeConversionFailed,
        MessageTooLarge,
        SocketCreateFailed,
        InvalidAddress,
        ConnectionFailed,
        SocketSendFailed
    };

    std::string_view to_string(JournalStatus status) noexcept;
}