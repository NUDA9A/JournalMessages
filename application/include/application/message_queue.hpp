#pragma once

#include <application/journal_command.hpp>

#include <optional>
#include <deque>
#include <mutex>
#include <condition_variable>

namespace application
{
    class MessageQueue
    {
    public:
        MessageQueue() = default;

        MessageQueue(const MessageQueue&) = delete;
        MessageQueue& operator=(const MessageQueue&) = delete;
        MessageQueue(MessageQueue&&) = delete;
        MessageQueue& operator=(MessageQueue&&) = delete;

        ~MessageQueue() = default;

        bool push(JournalCommand&& message);
        std::optional<JournalCommand> wait_and_pop();
        void stop() noexcept;
    private:
        std::deque<JournalCommand> queue_;
        std::mutex mutex_;
        std::condition_variable cv_;
        bool stopped_{false};
    };
}
