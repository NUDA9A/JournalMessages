#include <application/message_queue.hpp>

#include <utility>

namespace application
{
    bool MessageQueue::push(JournalCommand&& message)
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (stopped_)
            {
                return false;
            }
            queue_.push_back(std::move(message));
        }

        cv_.notify_one();

        return true;
    }

    std::optional<JournalCommand> MessageQueue::wait_and_pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [&]() { return stopped_ || !queue_.empty(); });
        if (queue_.empty())
        {
            return std::nullopt;
        }

        auto res = std::move(queue_.front());
        queue_.pop_front();
        return std::move(res);
    }

    void MessageQueue::stop() noexcept
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stopped_ = true;
        }

        cv_.notify_all();
    }
}
