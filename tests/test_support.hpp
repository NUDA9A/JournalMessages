#pragma once

#include <cstddef>
#include <string_view>
#include <iostream>
#include <optional>


namespace journal::tests
{
    struct TestContext
    {
        std::size_t checksCount{};
        std::size_t failuresCount{};

        bool check(bool condition, const std::string_view testName)
        {
            checksCount++;
            if (!condition)
            {
                failuresCount++;
                std::cerr << "FAILED: " << testName << std::endl;
            }

            return condition;
        }

        template <typename Result, typename Required>
        bool check_equal(const Result& result, const Required& required, const std::string_view testName)
        {
            return check(result == required, testName);
        }

        template <typename Required>
        bool check_equal(const std::optional<Required>& optionalResult, const Required& required, const std::string_view testName)
        {
            if (!optionalResult.has_value())
            {
                return check(false, testName);
            }

            return check_equal(*optionalResult, required, testName);
        }

        bool passed() const
        {
            return failuresCount == 0;
        }

        void print_summary() const
        {
            std::cerr
            << "Checks: " << checksCount
            << "\nPassed: " << checksCount - failuresCount
            << "\nFailed: " << failuresCount
            << std::endl;
        }
    };
}