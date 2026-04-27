#pragma once

#include <cstddef>
#include <vector>

#include <gtest/gtest.h>

namespace test {

inline void expect_near(float actual,
                        float expected,
                        float eps,
                        const char* expr)
{
    EXPECT_NEAR(actual, expected, eps) << expr;
}

inline void expect_vector_near(const std::vector<float>& actual,
                               const std::vector<float>& expected,
                               float eps,
                               const char* expr)
{
    ASSERT_EQ(actual.size(), expected.size()) << expr << " size mismatch";
    for (std::size_t i = 0; i < actual.size(); ++i) {
        EXPECT_NEAR(actual[i], expected[i], eps)
            << expr << " mismatch at index " << i;
    }
}

} // namespace test
