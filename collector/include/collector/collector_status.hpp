#pragma once

#include <string_view>

namespace collector
{
    enum class CollectorStatus
    {
        Success,
        SocketCreateFailed,
        SetReuseAddrFailed,
        InvalidIpAddress,
        BindFailed,
        ListenFailed,
        AcceptFailed,
        ReadFailed,
        ClientDisconnected,
        BadPacket,
        MessageTooLarge,
    };

    inline std::string_view to_string(CollectorStatus status) noexcept
    {
        switch (status)
        {
        case CollectorStatus::Success:
            return "SUCCESS";
        case CollectorStatus::SocketCreateFailed:
            return "SOCKET_CREATE_FAILED";
        case CollectorStatus::SetReuseAddrFailed:
            return "SET_REUSEADDR_FAILED";
        case CollectorStatus::InvalidIpAddress:
            return "INVALID_IP_ADDRESS";
        case CollectorStatus::BindFailed:
            return "BIND_FAILED";
        case CollectorStatus::ListenFailed:
            return "LISTEN_FAILED";
        case CollectorStatus::AcceptFailed:
            return "ACCEPT_FAILED";
        case CollectorStatus::ReadFailed:
            return "READ_FAILED";
        case CollectorStatus::ClientDisconnected:
            return "CLIENT_DISCONNECTED";
        case CollectorStatus::BadPacket:
            return "BAD_PACKET";
        case CollectorStatus::MessageTooLarge:
            return "MESSAGE_TOO_LARGE";
        default:
            return "UNKNOWN_COLLECTOR_STATSUS";
        }
    }
}