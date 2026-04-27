#include "test_utils.hpp"

#include <iostream>

std::vector<test::TestCase> matmul_tests();

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
    return test::run_tests(matmul_tests());
}
