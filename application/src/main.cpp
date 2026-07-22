#include <application/message_queue.hpp>
#include <journal/file_journal.hpp>
#include <journal/socket_journal.hpp>

#include <iostream>
#include <filesystem>
#include <thread>
#include <cstdint>
#include <charconv>
#include <system_error>
#include <cstring>
#include <utility>
#include <memory>
#include <string_view>
#include <string>
#include <variant>
#include <optional>
#include <chrono>

static void processUserMessage(application::MessageQueue& messageQueue)
{
    application::PendingMessage msg;

    std::string message;
    std::cout << "Write message:\n>> ";
    getline(std::cin, message);
    msg.message = message;
    msg.timestamp = std::chrono::system_clock::now();

    std::cout << "Want to specify LogLevel? Press enter if not.\n>> ";

    std::string logLevel;
    getline(std::cin, logLevel);

    if (!logLevel.empty())
    {
        const auto userLogLevel = journal::to_log_level(logLevel);
        if (!userLogLevel)
        {
            std::cerr << "[ERROR]: Invalid LogLevel value: " << logLevel << ". Valid values are: info, warning, error" << std::endl;
            std::cerr << "Try again!" << std::endl;
            return;
        }

        msg.level = *userLogLevel;
    }

    if (!messageQueue.push({std::move(msg)}))
    {
        std::cerr << "[WARNING]: Exiting! This message won't be logged!" << std::endl;
    }
}

static void changeLogLevel(application::MessageQueue& messageQueue)
{
    while (true)
    {
        std::string logLevel;
        std::cout << "Enter new LogLevel. Valid values are: info, warning, error.\n>> " << std::endl;
        getline(std::cin, logLevel);
        const auto userLogLevel = journal::to_log_level(logLevel);
        if (!userLogLevel)
        {
            std::cerr << "[ERROR]: Invalid LogLevel value: " << logLevel << ". Valid values are: info, warning, error" << std::endl;
            std::cerr << "Try again!" << std::endl;
            continue;
        }

        if (!messageQueue.push({*userLogLevel}))
        {
            std::cerr << "[WARNING]: Exiting! LogLevel won't be changed!" << std::endl;
        }

        return;
    }
}

static void writingProc(journal::Journal* journalInstance, application::MessageQueue& messageQueue)
{
    while (true)
    {
        const auto record = messageQueue.wait_and_pop();
        if (!record)
        {
            return;
        }

        if (std::holds_alternative<journal::LogLevel>(record->command))
        {
            const auto logLevel = std::get<journal::LogLevel>(record->command);
            journalInstance->setLogLevel(logLevel);
            std::cout << "Set new LogLevel: " << journal::to_string(logLevel) << std::endl;
            continue;
        }

        const auto& msgRecord = std::get<application::PendingMessage>(record->command);

        if (msgRecord.level)
        {
            journalInstance->write(msgRecord.message, msgRecord.timestamp, *(msgRecord.level));
        } else
        {
            journalInstance->write(msgRecord.message, msgRecord.timestamp);
        }

        if (journalInstance->status() != journal::JournalStatus::Success)
        {
            std::cerr << "[WARNING]: This message won't be written into log. " << journal::to_string(journalInstance->status()) << std::endl;
        }
    }
}

static std::unique_ptr<journal::Journal> makeJournal(const std::string& address, const std::uint16_t port, const journal::LogLevel logLevel, const int argc)
{
    if (argc == 3)
    {
        return std::make_unique<journal::FileJournal>(std::filesystem::path(address), logLevel);
    }

    return std::make_unique<journal::SocketJournal>(address, port, logLevel);
}

template <typename T>
std::optional<T> parseNumFromString(const char* str, std::size_t size, const std::string_view name)
{
    T res{};

    auto [ptr, ec] = std::from_chars(str, str + size, res);
    if (ec != std::errc() || ptr != str + size)
    {
        std::cerr << "[ERROR]: Invalid " << name << ": " << str << "." << std::endl;
        return std::nullopt;
    }

    return res;
}

int main(const int argc, char* argv[])
{
    if (argc != 3 && argc != 4)
    {
        std::cerr << "[ERROR]: Usage ./JournalMessages <path/to/log_file> <default_log_level> for FileJournal" << std::endl;
        std::cerr << "               ./JournalMessages <ip_address> <port> <default_log_level> for SocketJournal" << std::endl;
        return 1;
    }

    const std::string address = argv[1];
    std::uint16_t port = 0;

    if (argc == 4)
    {
        const auto optPort = parseNumFromString<std::uint16_t>(argv[2], strlen(argv[2]), "port");
        if (!optPort || *optPort == 0)
        {
            return 2;
        }
        port = *optPort;
    }

    const auto default_log_level = journal::to_log_level(argc == 3 ? argv[2] : argv[3]);
    if (!default_log_level)
    {
        std::cerr << "[ERROR]: Invalid LogLevel value: " << (argc == 3 ? argv[2] : argv[3]) << "." << std::endl << "Valid values are: info, warning, error" << std::endl;
        return 3;
    }

    const auto journalInstance = makeJournal(address, port, *default_log_level, argc);

    if (journalInstance->status() != journal::JournalStatus::Success)
    {
        std::cerr << "[ERROR]: Can not create journal: " << journal::to_string(journalInstance->status()) << std::endl;
        return 4;
    }

    application::MessageQueue messageQueue;
    std::thread writingThread{[&]()
    {
        writingProc(journalInstance.get(), messageQueue);
    }};

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        std::cout << "Enter command number (1-3):" << std::endl;
        std::cout << "1. Write message." << std::endl;
        std::cout << "2. Change LogLevel." << std::endl;
        std::cout << "3. Exit." << std::endl;
        std::cout << ">> ";

        std::string commandLine;
        std::getline(std::cin, commandLine);

        const auto optCommand = parseNumFromString<int>(commandLine.c_str(), commandLine.size(), "command");

        if (!optCommand)
        {
            continue;
        }

        switch (*optCommand)
        {
        case 1:
            processUserMessage(messageQueue);
            break;
        case 2:
            changeLogLevel(messageQueue);
            break;
        case 3:
            goto end;
        default:
            std::cerr << "[ERROR]: Invalid command! Try again!" << std::endl;
        }
    }

end:
    messageQueue.stop();
    writingThread.join();

    return 0;
}