#pragma once

#include <cstddef>
#include <deque>
#include <chrono>
#include <limits>

#include <collector/received_log_record.hpp>

namespace collector::statistics
{
    struct Statistics
    {
        std::size_t msgCount{};
        std::size_t infoMsgCount{};
        std::size_t warningMsgCount{};
        std::size_t errorMsgCount{};
        std::size_t minMsgLength{std::numeric_limits<std::size_t>::max()};
        std::size_t maxMsgLength{};
        std::size_t msgLengthSum{};
        bool changed{false};

        std::deque<std::chrono::steady_clock::time_point> msgTimestampQueue;

        void processRecord(const ReceivedLogRecord& record);
        void updateQueue(std::chrono::steady_clock::time_point timestamp);
        void markUnchanged() noexcept;
    };
}