#include <collector/tcp_server.hpp>
#include <collector/statistics.hpp>
#include <journal/log_level.hpp>

#include <iostream>
#include <string>
#include <optional>
#include <cstddef>
#include <stdexcept>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>


template <typename T>
std::optional<T> parseNumFromString(const char* str, const std::size_t size, const std::string_view name)
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

template <typename T>
int parseNumArg(const char* str, const std::size_t size, const std::string_view name, T& arg, int& errorCode)
{
    const auto argOpt = parseNumFromString<T>(str, size, name);

    if (!argOpt)
    {
        return errorCode;
    }
    errorCode++;

    arg = *argOpt;

    return 0;
}

static void printStatistics(const collector::statistics::Statistics& statistics)
{
    std::cout << "[INFO]: Statistics:" << std::endl;
    std::cout << "Total messages: " << statistics.msgCount << std::endl;
    std::cout << "Info messages: " << statistics.infoMsgCount << std::endl;
    std::cout << "Warning messages: " << statistics.warningMsgCount << std::endl;
    std::cout << "Error messages: " << statistics.errorMsgCount << std::endl;
    if (statistics.msgCount != 0)
    {
        std::cout << "Min message's length: " << statistics.minMsgLength << std::endl;
        std::cout << "Max message's length: " << statistics.maxMsgLength << std::endl;
        std::cout << "Avg message's length: " << (statistics.msgLengthSum / static_cast<double>(statistics.msgCount)) << std::endl;
    }
    std::cout << "Messages in 1 hour: " << statistics.msgTimestampQueue.size() << std::endl;
}

static void timerProc(std::mutex& mtx, std::condition_variable& cv, collector::statistics::Statistics& statistics, bool& stopRequested, const std::int64_t T)
{
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::seconds(T), [&]() { return stopRequested; });

        if (stopRequested)
        {
            return;
        }

        statistics.updateQueue(std::chrono::steady_clock::now());

        if (statistics.changed)
        {
            printStatistics(statistics);
            statistics.markUnchanged();
        }
    }
}

int main(const int argc, const char*  argv[])
{
    int errorCode = 1;
    if (argc != 5)
    {
        std::cerr << "[ERROR]: Usage ./JournalCollector <ipAddress> <port> <N> <T>" << std::endl;
        return errorCode;
    }
    errorCode++;

    const std::string ipAddress = argv[1];
    std::uint16_t port{};
    std::size_t N{};
    std::int64_t T{};

    if (parseNumArg<std::uint16_t>(argv[2], std::strlen(argv[2]), "port", port, errorCode) != 0 || port == 0)
    {
        return errorCode;
    }

    if (parseNumArg<std::size_t>(argv[3], std::strlen(argv[3]), "N", N, errorCode) != 0 || N == 0)
    {
        return errorCode;
    }

    if (parseNumArg<std::int64_t>(argv[4], std::strlen(argv[4]), "T", T, errorCode) != 0 || T <= 0)
    {
        return errorCode;
    }

    collector::TCPServer server{ipAddress, port};
    if (server.status() != collector::CollectorStatus::Success)
    {
        std::cerr << "[ERROR]: Can not create TCPServer: " << collector::to_string(server.status()) << std::endl;
        return errorCode;
    }
    errorCode++;

    collector::statistics::Statistics statistics{};

    server.acceptClient();
    if (server.status() != collector::CollectorStatus::Success)
    {
        std::cerr << "[ERROR]: Can not accept client: " << collector::to_string(server.status()) << std::endl;
        return errorCode;
    }
    errorCode++;

    std::mutex mtx;

    bool stopRequested = false;
    std::condition_variable cv;
    std::thread timerThread{[&]()
    {
        timerProc(mtx, cv, statistics, stopRequested, T);
    }};

    while (server.status() == collector::CollectorStatus::Success)
    {
        const auto recordOpt = server.receive();
        if (!recordOpt)
        {
            std::unique_lock lock(mtx);
            std::cerr << "[WARNING]: Something went wrong during receiving a message!" << std::endl;
            continue;
        }
        const auto& record = *recordOpt;

        const auto currentTime = std::chrono::system_clock::to_time_t(record.timestamp);
        const std::tm* currentDateTime = std::localtime(&currentTime);

        if (currentDateTime == nullptr)
        {
            std::unique_lock lock(mtx);
            std::cerr << "[WARNING]: Can not get localtime! Date-time won't be printed!" << std::endl;
        }

        std::ostringstream oss;

        if (currentDateTime)
        {
            oss << std::put_time(currentDateTime, "[%Y-%m-%d %H:%M:%S] ");
        }

        oss << "[" << journal::to_string(record.level) << "] " << record.message << "\n";

        {
            std::unique_lock lock(mtx);
            std::cout << oss.str();
            statistics.processRecord(record);
            if (statistics.msgCount % N == 0)
            {
                printStatistics(statistics);
                statistics.markUnchanged();
            }
        }
    }

    {
        std::unique_lock lock(mtx);
        stopRequested = true;
    }
    cv.notify_all();
    timerThread.join();

    if (server.status() == collector::CollectorStatus::ClientDisconnected)
    {
        std::cerr << "[WARNING]: Client disconnected! Exiting..." << std::endl;
    } else
    {
        std::cerr << "[ERROR]: Something went wrong: " << collector::to_string(server.status()) << std::endl;
        return errorCode;
    }

    errorCode++;

    return 0;
}