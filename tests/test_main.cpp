#include "tests_suites.hpp"

int main()
{
    journal::tests::TestContext testContext{};

    journal::tests::run_log_level_tests(testContext);
    journal::tests::run_file_journal_tests(testContext);
    journal::tests::run_message_queue_tests(testContext);

#if (JOURNAL_ENABLE_SOCKET)
    journal::tests::run_socket_journal_integration_tests(testContext);
#endif

    testContext.print_summary();

    return testContext.passed() ? 0 : 1;
}