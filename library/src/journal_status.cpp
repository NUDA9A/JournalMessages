#include <journal/journal_status.hpp>

namespace journal
{
    std::string_view to_string(const JournalStatus status) noexcept
    {
        switch (status)
        {
        case JournalStatus::Success:
            return "SUCCESS";
        case JournalStatus::FilteredOut:
            return "FILTERED_OUT";
        case JournalStatus::InvalidArgument:
            return "INVALID_ARGUMENT";
        case JournalStatus::FileOpenFailed:
            return "FILE_OPEN_FAILED";
        case JournalStatus::FileWriteFailed:
            return "FILE_WRITE_FAILED";
        case JournalStatus::TimeConversionFailed:
            return "TIME_CONVERSION_FAILED";
        case JournalStatus::MessageTooLarge:
            return "MESSAGE_TOO_LARGE";
        case JournalStatus::SocketCreateFailed:
            return "SOCKET_CREATE_FAILED";
        case JournalStatus::InvalidAddress:
            return "INVALID_ADDRESS";
        case JournalStatus::ConnectionFailed:
            return "CONNECTION_FAILED";
        case JournalStatus::SocketSendFailed:
            return "SOCKET_SEND_FAILED";
        default:
            return "UNKNOWN_JOURNAL_STATUS";
        }
    }
}
