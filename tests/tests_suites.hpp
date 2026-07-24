#pragma once

#include "test_support.hpp"

namespace journal::tests
{
    void run_log_level_tests(TestContext&);
    void run_file_journal_tests(TestContext&);
    void run_message_queue_tests(TestContext&);

#if (JOURNAL_ENABLE_SOCKET)
    void run_socket_journal_integration_tests(TestContext&);
#endif
}