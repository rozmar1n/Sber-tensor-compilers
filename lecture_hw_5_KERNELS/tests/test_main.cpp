#include "test_utils.hpp"

#include <iostream>

std::vector<test::TestCase> matmul_tests();
std::vector<test::TestCase> conv2d_tests();

namespace test {

int run_tests(const std::vector<TestCase>& tests) {
    int failures = 0;
    for (const auto& test : tests) {
        try {
            test.run();
            std::cout << "[PASS] " << test.name << '\n';
        } catch (const std::exception& ex) {
            ++failures;
            std::cerr << "[FAIL] " << test.name << ": " << ex.what() << '\n';
        }
    }
    return failures == 0 ? 0 : 1;
}

}  // namespace test

int main() {
    std::vector<test::TestCase> tests = matmul_tests();
    const std::vector<test::TestCase> conv_tests = conv2d_tests();
    tests.insert(tests.end(), conv_tests.begin(), conv_tests.end());
    return test::run_tests(tests);
}
