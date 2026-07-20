#include "tests_suites.hpp"
#include <journal/log_level.hpp>

namespace journal::tests
{
    static void to_log_level_correctness_test(TestContext& testContext)
    {
        const auto logLevelInfo = to_log_level("info");
        const auto logLevelWarning = to_log_level("warning");
        const auto logLevelError = to_log_level("error");

        testContext.check_equal(logLevelInfo, LogLevel::Info, "to_log_level() correctness: to_log_level(\"info\") != LogLevel::Info");
        testContext.check_equal(logLevelWarning, LogLevel::Warning, "to_log_level() correctness: to_log_level(\"warning\") != LogLevel::Warning");
        testContext.check_equal(logLevelError, LogLevel::Error, "to_log_level() correctness: to_log_level(\"error\") != LogLevel::Error");
    }

    static void to_log_level_case_insensitive_test(TestContext& testContext)
    {
        const auto logLevelInfo = to_log_level("INFO");
        const auto logLevelWarning = to_log_level("WaRnInG");
        const auto logLevelError = to_log_level("Error");
        const auto logLevelError2 = to_log_level("eRROR");

        testContext.check_equal(logLevelInfo, LogLevel::Info, "to_log_level() case insensitivity: to_log_level(\"INFO\") != LogLevel::Info");
        testContext.check_equal(logLevelWarning, LogLevel::Warning, "to_log_level() case insensitivity: to_log_level(\"WaRnInG\") != LogLevel::Warning");
        testContext.check_equal(logLevelError, LogLevel::Error, "to_log_level() case insensitivity: to_log_level(\"Error\") != LogLevel::Error");
        testContext.check_equal(logLevelError2, LogLevel::Error, "to_log_level() case insensitivity: to_log_level(\"eRROR\") != LogLevel::Error");
    }

    static void to_log_level_incorrect_log_level_test(TestContext& testContext)
    {
        const auto emptyStr = to_log_level("");
        const auto unknownStr = to_log_level("unknown");
        const auto invalidStr = to_log_level("inva lid string");

        testContext.check_equal(emptyStr, std::nullopt, "to_log_level() incorrect input: to_log_level(\"\") != std::nullopt");
        testContext.check_equal(unknownStr, std::nullopt, "to_log_level() incorrect input: to_log_level(\"unknown\") != std::nullopt");
        testContext.check_equal(invalidStr, std::nullopt, "to_log_level() incorrect input: to_log_level(\"inva lid string\") != std::nullopt");
    }

    static void is_compatible_test(TestContext& testContext)
    {
        const bool infoVsInfo = is_compatible(LogLevel::Info, LogLevel::Info);
        const bool infoVsWarning = is_compatible(LogLevel::Info, LogLevel::Warning);
        const bool infoVsError = is_compatible(LogLevel::Info, LogLevel::Error);
        const bool warningVsInfo = is_compatible(LogLevel::Warning, LogLevel::Info);
        const bool warningVsWarning = is_compatible(LogLevel::Warning, LogLevel::Warning);
        const bool warningVsError = is_compatible(LogLevel::Warning, LogLevel::Error);
        const bool errorVsInfo = is_compatible(LogLevel::Error, LogLevel::Info);
        const bool errorVsWarning = is_compatible(LogLevel::Error, LogLevel::Warning);
        const bool errorVsError = is_compatible(LogLevel::Error, LogLevel::Error);

        testContext.check_equal(infoVsInfo, true, "is_compatible() correctness: is_compatible(LogLevel::Info, LogLevel::Info) != true");
        testContext.check_equal(infoVsWarning, false, "is_compatible() correctness: is_compatible(LogLevel::Info, LogLevel::Warning) != false");
        testContext.check_equal(infoVsError, false, "is_compatible() correctness: is_compatible(LogLevel::Info, LogLevel::Error) != false");
        testContext.check_equal(warningVsInfo, true, "is_compatible() correctness: is_compatible(LogLevel::Warning, LogLevel::Info) != true");
        testContext.check_equal(warningVsWarning, true, "is_compatible() correctness: is_compatible(LogLevel::Warning, LogLevel::Warning) != true");
        testContext.check_equal(warningVsError, false, "is_compatible() correctness: is_compatible(LogLevel::Warning, LogLevel::Error) != false");
        testContext.check_equal(errorVsInfo, true, "is_compatible() correctness: is_compatible(LogLevel::Error, LogLevel::Info) != true");
        testContext.check_equal(errorVsWarning, true, "is_compatible() correctness: is_compatible(LogLevel::Error, LogLevel::Warning) != true");
        testContext.check_equal(errorVsError, true, "is_compatible() correctness: is_compatible(LogLevel::Error, LogLevel::Error) != true");
    }

    void run_log_level_tests(TestContext& testContext)
    {
        to_log_level_correctness_test(testContext);
        to_log_level_case_insensitive_test(testContext);
        to_log_level_incorrect_log_level_test(testContext);
        is_compatible_test(testContext);
    }
}