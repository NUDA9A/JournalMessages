#pragma once

#include <application/pending_message.hpp>

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

        bool push(PendingMessage&& message);
        std::optional<PendingMessage> wait_and_pop();
        void stop() noexcept;
    private:
        std::deque<PendingMessage> queue_;
        std::mutex mutex_;
        std::condition_variable cv_;
        bool stopped_{false};
    };
}
