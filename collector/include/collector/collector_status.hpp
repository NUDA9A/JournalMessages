#pragma once

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
}