#include "kernels/matmul.hpp"
#include "test_utils.hpp"

#include <vector>

namespace {

constexpr float kEps = 1e-4f;

void matmul_1x1() {
    const std::vector<float> A{2.0f};
    const std::vector<float> B{3.0f};
    std::vector<float> C(1, 0.0f);

    kernels::matmul_naive(A.data(), B.data(), C.data(), 1, 1, 1);

    test::expect_near(C[0], 6.0f, kEps, "1x1 matmul");
}

void matmul_2x2_manual() {
    const std::vector<float> A{1.0f, 2.0f, 3.0f, 4.0f};
    const std::vector<float> B{5.0f, 6.0f, 7.0f, 8.0f};
    std::vector<float> C(4, 0.0f);

    kernels::matmul_naive(A.data(), B.data(), C.data(), 2, 2, 2);

    test::expect_vector_near(C, {19.0f, 22.0f, 43.0f, 50.0f}, kEps, "2x2 matmul");
}

void matmul_rectangular_2x3_3x2() {
    const std::vector<float> A{
        1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f,
    };
    const std::vector<float> B{
        7.0f, 8.0f,
        9.0f, 10.0f,
        11.0f, 12.0f,
    };
    std::vector<float> C(4, 0.0f);

    kernels::matmul_naive(A.data(), B.data(), C.data(), 2, 3, 2);

    test::expect_vector_near(C, {58.0f, 64.0f, 139.0f, 154.0f}, kEps,
                             "rectangular matmul");
}

}  // namespace

std::vector<test::TestCase> matmul_tests() {
    return {
        {"matmul_naive computes 1x1", matmul_1x1},
        {"matmul_naive computes 2x2 manual example", matmul_2x2_manual},
        {"matmul_naive computes rectangular 2x3 by 3x2", matmul_rectangular_2x3_3x2},
    };
}
