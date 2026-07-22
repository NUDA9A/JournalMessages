#pragma once

#include <journal/log_record.hpp>
#include <journal/journal_status.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>
#include <chrono>
#include <cstring>
#include <limits>

namespace journal
{
    constexpr std::size_t HEADER_SIZE = 13;

    inline JournalStatus serialize(const LogRecordView& record, std::vector<std::byte>& outputBuffer)
    {
        const std::uint64_t timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(record.timestamp.time_since_epoch()).count();
        if (record.message.size() > std::numeric_limits<std::uint32_t>::max())
        {
            return JournalStatus::MessageTooLarge;
        }
        const std::uint32_t message_size = static_cast<std::uint32_t>(record.message.size());
        const std::uint8_t log_level = static_cast<std::uint8_t>(record.level);

        outputBuffer.resize(HEADER_SIZE + message_size);

        for (std::size_t i = 0; i < 4; ++i)
        {
            outputBuffer[i] = static_cast<std::byte>((message_size >> 8 * (4 - (i + 1))) & 0xFF);
        }

        outputBuffer[4] = static_cast<std::byte>(log_level);

        for (std::size_t i = 0; i < 8; ++i)
        {
            outputBuffer[5 + i] = static_cast<std::byte>((timestamp_ms >> 8 * (8 - (i + 1))) & 0xFF);
        }

        if (!record.message.empty())
        {
            std::memcpy(outputBuffer.data() + HEADER_SIZE, record.message.data(), record.message.size());
        }

        return JournalStatus::Success;
    }
}
