#include "tests_suites.hpp"

#include <application/message_queue.hpp>

#include <chrono>
#include <optional>
#include <thread>
#include <utility>
#include <variant>

namespace journal::tests
{
    static void message_queue_fifo_test(TestContext& testContext)
    {
        application::MessageQueue messageQueue;

        const auto timestamp = std::chrono::system_clock::now();
        application::PendingMessage message{
            "Test message",
            timestamp,
            LogLevel::Warning
        };

        const bool firstPushed = messageQueue.push({std::move(message)});
        const bool secondPushed = messageQueue.push({LogLevel::Error});

        testContext.check(firstPushed, "MessageQueue FIFO test: first push() returned false");
        testContext.check(secondPushed, "MessageQueue FIFO test: second push() returned false");

        const auto firstCommand = messageQueue.wait_and_pop();
        const auto secondCommand = messageQueue.wait_and_pop();

        testContext.check(firstCommand.has_value(), "MessageQueue FIFO test: first command is empty");
        testContext.check(secondCommand.has_value(), "MessageQueue FIFO test: second command is empty");

        const auto* receivedMessage = firstCommand
            ? std::get_if<application::PendingMessage>(&firstCommand->command)
            : nullptr;

        testContext.check(receivedMessage != nullptr, "MessageQueue FIFO test: first command is not PendingMessage");

        if (receivedMessage != nullptr)
        {
            testContext.check_equal(receivedMessage->message, "Test message", "MessageQueue FIFO test: incorrect message text");
            testContext.check_equal(receivedMessage->timestamp, timestamp, "MessageQueue FIFO test: incorrect timestamp");
            testContext.check_equal(receivedMessage->level, LogLevel::Warning, "MessageQueue FIFO test: incorrect message LogLevel");
        }

        const auto* receivedLogLevel = secondCommand
            ? std::get_if<LogLevel>(&secondCommand->command)
            : nullptr;

        testContext.check(receivedLogLevel != nullptr, "MessageQueue FIFO test: second command is not LogLevel");

        if (receivedLogLevel != nullptr)
        {
            testContext.check_equal(*receivedLogLevel, LogLevel::Error, "MessageQueue FIFO test: incorrect second command");
        }
    }

    static void message_queue_threaded_transfer_test(TestContext& testContext)
    {
        application::MessageQueue messageQueue;
        std::optional<application::JournalCommand> receivedCommand;

        std::thread consumer{
            [&]()
            {
                receivedCommand = messageQueue.wait_and_pop();
            }
        };

        const bool pushed = messageQueue.push({LogLevel::Warning});

        consumer.join();

        testContext.check(pushed, "MessageQueue threaded transfer test: push() returned false");
        testContext.check(receivedCommand.has_value(), "MessageQueue threaded transfer test: command was not received");

        const auto* receivedLogLevel = receivedCommand
            ? std::get_if<LogLevel>(&receivedCommand->command)
            : nullptr;

        testContext.check(receivedLogLevel != nullptr, "MessageQueue threaded transfer test: received command is not LogLevel");

        if (receivedLogLevel != nullptr)
        {
            testContext.check_equal(*receivedLogLevel, LogLevel::Warning, "MessageQueue threaded transfer test: incorrect LogLevel");
        }
    }

    static void message_queue_stop_test(TestContext& testContext)
    {
        application::MessageQueue messageQueue;
        std::optional<application::JournalCommand> receivedCommand;

        std::thread consumer{
            [&]()
            {
                receivedCommand = messageQueue.wait_and_pop();
            }
        };

        messageQueue.stop();
        consumer.join();

        const bool pushedAfterStop = messageQueue.push({LogLevel::Info});

        testContext.check(!receivedCommand.has_value(), "MessageQueue stop test: wait_and_pop() returned a command after stop()");
        testContext.check(!pushedAfterStop, "MessageQueue stop test: push() succeeded after stop()");
    }

    void run_message_queue_tests(TestContext& testContext)
    {
        message_queue_fifo_test(testContext);
        message_queue_threaded_transfer_test(testContext);
        message_queue_stop_test(testContext);
    }
}