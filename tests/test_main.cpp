#include "tests_suites.hpp"

int main()
{
    journal::tests::TestContext testContext{};

    journal::tests::run_log_level_tests(testContext);
    journal::tests::run_file_journal_tests(testContext);

    testContext.print_summary();

    return testContext.passed() ? 0 : 1;
}