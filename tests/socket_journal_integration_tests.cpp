#include "tests_suites.hpp"

#include <collector/tcp_server.hpp>
#include <journal/socket_journal.hpp>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <optional>

namespace
{
    std::optional<std::uint16_t> find_available_port()
    {
        const int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
        {
            return std::nullopt;
        }

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        address.sin_port = 0;

        if (bind(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0)
        {
            close(fd);
            return std::nullopt;
        }

        socklen_t addressSize = sizeof(address);
        if (getsockname(fd, reinterpret_cast<sockaddr*>(&address), &addressSize) < 0)
        {
            close(fd);
            return std::nullopt;
        }

        const std::uint16_t port = ntohs(address.sin_port);
        close(fd);

        if (port == 0)
        {
            return std::nullopt;
        }

        return port;
    }
}

namespace journal::tests
{
    static void socket_journal_tcp_server_test(TestContext& testContext)
    {
        const auto port = find_available_port();

        testContext.check(port.has_value(), "SocketJournal -> TCPServer test: can not find available port");

        if (!port)
        {
            return;
        }

        collector::TCPServer server{"127.0.0.1", *port};

        testContext.check_equal(
            server.status(),
            collector::CollectorStatus::Success,
            "SocketJournal -> TCPServer test: TCPServer initialization failed"
        );

        if (server.status() != collector::CollectorStatus::Success)
        {
            return;
        }

        SocketJournal socketJournal{"127.0.0.1", *port, LogLevel::Info};

        testContext.check(socketJournal.is_ready(), "SocketJournal -> TCPServer test: SocketJournal initialization failed");
        testContext.check_equal(
            socketJournal.status(),
            JournalStatus::Success,
            "SocketJournal -> TCPServer test: incorrect SocketJournal status after initialization"
        );

        if (!socketJournal.is_ready())
        {
            return;
        }

        server.acceptClient();

        testContext.check_equal(
            server.status(),
            collector::CollectorStatus::Success,
            "SocketJournal -> TCPServer test: acceptClient() failed"
        );

        if (server.status() != collector::CollectorStatus::Success)
        {
            return;
        }

        const auto timestamp = std::chrono::system_clock::time_point{
            std::chrono::milliseconds{1'700'000'000'123}
        };

        socketJournal.write("Socket test message", timestamp, LogLevel::Warning);

        testContext.check_equal(
            socketJournal.status(),
            JournalStatus::Success,
            "SocketJournal -> TCPServer test: SocketJournal::write() failed"
        );

        const auto receivedRecord = server.receive();

        testContext.check(
            receivedRecord.has_value(),
            "SocketJournal -> TCPServer test: TCPServer did not receive record"
        );

        if (!receivedRecord)
        {
            return;
        }

        testContext.check_equal(
            receivedRecord->message,
            "Socket test message",
            "SocketJournal -> TCPServer test: incorrect message"
        );

        testContext.check_equal(
            receivedRecord->level,
            LogLevel::Warning,
            "SocketJournal -> TCPServer test: incorrect LogLevel"
        );

        testContext.check_equal(
            receivedRecord->timestamp,
            timestamp,
            "SocketJournal -> TCPServer test: incorrect timestamp"
        );
    }

    void run_socket_journal_integration_tests(TestContext& testContext)
    {
        socket_journal_tcp_server_test(testContext);
    }
}