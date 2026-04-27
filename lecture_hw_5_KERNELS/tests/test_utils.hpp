#pragma once

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace test {

using TestFn = void (*)();

struct TestCase
{
    const char* name;
    TestFn run;
};

inline void expect_near(float actual,
                        float expected,
                        float eps,
                        const char* expr)
{
    if (std::fabs(actual - expected) > eps) {
        throw std::runtime_error(std::string(expr) + " expected " +
                                 std::to_string(expected) + ", got " +
                                 std::to_string(actual));
    }
}

inline void expect_vector_near(const std::vector<float>& actual,
                               const std::vector<float>& expected,
                               float eps,
                               const char* expr)
{
    if (actual.size() != expected.size()) {
        throw std::runtime_error(std::string(expr) + " size mismatch");
    }
    for (std::size_t i = 0; i < actual.size(); ++i) {
        if (std::fabs(actual[i] - expected[i]) > eps) {
            throw std::runtime_error(std::string(expr) + " mismatch at index " +
                                     std::to_string(i) + ": expected " +
                                     std::to_string(expected[i]) + ", got " +
                                     std::to_string(actual[i]));
        }
    }
}

int run_tests(const std::vector<TestCase>& tests);

} // namespace test
