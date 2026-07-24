#include <collector/statistics.hpp>
#include <journal/log_level.hpp>

#include <algorithm>
#include <chrono>

namespace collector::statistics
{
    void Statistics::processRecord(const ReceivedLogRecord& record)
    {
        msgCount++;

        switch (record.level)
        {
        case journal::LogLevel::Info:
            infoMsgCount++;
            break;
        case journal::LogLevel::Warning:
            warningMsgCount++;
            break;
        case journal::LogLevel::Error:
            errorMsgCount++;
            break;
        default:
            msgCount--;
            return;
        }

        minMsgLength = std::min(minMsgLength, record.message.size());
        maxMsgLength = std::max(maxMsgLength, record.message.size());
        msgLengthSum += record.message.size();

        const auto now = std::chrono::steady_clock::now();
        updateQueue(now);
        msgTimestampQueue.push_back(now);

        changed = true;
    }

    void Statistics::updateQueue(const std::chrono::steady_clock::time_point timestamp)
    {
        const auto beforeSize = msgTimestampQueue.size();

        while (!msgTimestampQueue.empty() &&  msgTimestampQueue.front() < timestamp - std::chrono::hours(1))
        {
            msgTimestampQueue.pop_front();
        }

        if (beforeSize != msgTimestampQueue.size())
        {
            changed = true;
        }
    }

    void Statistics::markUnchanged() noexcept
    {
        changed = false;
    }
}
