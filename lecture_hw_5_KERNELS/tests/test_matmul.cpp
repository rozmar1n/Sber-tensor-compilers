#include "kernels/matmul.hpp"
#include "test_utils.hpp"

#include <random>
#include <string>
#include <vector>

namespace {

constexpr float kEps = 1e-4f;
constexpr int kTileSize = 8;

using MatmulFn = void (*)(const float*, const float*, float*, int, int, int);

std::vector<float> sequential_matrix(int size,
                                     float scale = 1.0f,
                                     float offset = 0.0f)
{
    std::vector<float> values(size);
    for (int i = 0; i < size; ++i) {
        values[i] = offset + scale * static_cast<float>(i + 1);
    }
    return values;
}

std::vector<float> random_matrix(int size)
{
    std::mt19937 rng(12345);
    std::uniform_real_distribution<float> dist(-2.0f, 2.0f);
    std::vector<float> values(size);
    for (float& value : values) {
        value = dist(rng);
    }
    return values;
}

std::vector<float> identity_matrix(int size)
{
    std::vector<float> values(size * size, 0.0f);
    for (int i = 0; i < size; ++i) {
        values[i * size + i] = 1.0f;
    }
    return values;
}

void expect_variant_matches_naive(MatmulFn fn,
                                  const std::string& variant,
                                  const std::vector<float>& A,
                                  const std::vector<float>& B,
                                  int M,
                                  int K,
                                  int N)
{
    std::vector<float> expected(M * N, 0.0f);
    std::vector<float> actual(M * N, -1.0f);

    kernels::matmul_naive(A.data(), B.data(), expected.data(), M, K, N);
    fn(A.data(), B.data(), actual.data(), M, K, N);

    test::expect_vector_near(actual, expected, kEps, variant.c_str());
}

void expect_all_variants_match_naive(const std::vector<float>& A,
                                     const std::vector<float>& B,
                                     int M,
                                     int K,
                                     int N,
                                     const char* case_name)
{
    expect_variant_matches_naive(kernels::matmul_cache_friendly,
                                 std::string(case_name) + " cache-friendly",
                                 A,
                                 B,
                                 M,
                                 K,
                                 N);
    expect_variant_matches_naive(
        [](const float* A_ptr,
           const float* B_ptr,
           float* C_ptr,
           int m,
           int k,
           int n) {
            kernels::matmul_tiled(A_ptr, B_ptr, C_ptr, m, k, n, kTileSize);
        },
        std::string(case_name) + " tiled",
        A,
        B,
        M,
        K,
        N);
    expect_variant_matches_naive(kernels::matmul_vectorized,
                                 std::string(case_name) + " vectorized",
                                 A,
                                 B,
                                 M,
                                 K,
                                 N);
}

void matmul_1x1()
{
    const std::vector<float> A{ 2.0f };
    const std::vector<float> B{ 3.0f };
    std::vector<float> C(1, 0.0f);

    kernels::matmul_naive(A.data(), B.data(), C.data(), 1, 1, 1);

    test::expect_near(C[0], 6.0f, kEps, "1x1 matmul");
    expect_all_variants_match_naive(A, B, 1, 1, 1, "1x1");
}

void matmul_2x2_manual()
{
    const std::vector<float> A{ 1.0f, 2.0f, 3.0f, 4.0f };
    const std::vector<float> B{ 5.0f, 6.0f, 7.0f, 8.0f };
    std::vector<float> C(4, 0.0f);

    kernels::matmul_naive(A.data(), B.data(), C.data(), 2, 2, 2);

    test::expect_vector_near(
        C, { 19.0f, 22.0f, 43.0f, 50.0f }, kEps, "2x2 matmul");
    expect_all_variants_match_naive(A, B, 2, 2, 2, "2x2");
}

void matmul_rectangular_3x5_5x4()
{
    const std::vector<float> A = sequential_matrix(3 * 5, 0.25f, -1.0f);
    const std::vector<float> B = sequential_matrix(5 * 4, -0.125f, 2.0f);

    expect_all_variants_match_naive(A, B, 3, 5, 4, "rectangular 3x5 by 5x4");
}

void matmul_non_avx_tail_7x10_10x9()
{
    const std::vector<float> A = sequential_matrix(7 * 10, 0.1f, -3.0f);
    const std::vector<float> B = sequential_matrix(10 * 9, 0.05f, 1.0f);

    expect_all_variants_match_naive(
        A, B, 7, 10, 9, "non-AVX-tail 7x10 by 10x9");
}

void matmul_non_tile_aligned_31x17_17x23()
{
    const std::vector<float> A = sequential_matrix(31 * 17, 0.01f, -2.0f);
    const std::vector<float> B = sequential_matrix(17 * 23, -0.02f, 4.0f);

    expect_all_variants_match_naive(
        A, B, 31, 17, 23, "non-tile-aligned 31x17 by 17x23");
}

void matmul_zero_matrix()
{
    const std::vector<float> A(4 * 6, 0.0f);
    const std::vector<float> B = sequential_matrix(6 * 5, 0.125f, -1.0f);

    expect_all_variants_match_naive(A, B, 4, 6, 5, "zero matrix");
}

void matmul_identity_matrix()
{
    const std::vector<float> A = identity_matrix(6);
    const std::vector<float> B = sequential_matrix(6 * 6, 0.2f, -3.0f);

    expect_all_variants_match_naive(A, B, 6, 6, 6, "identity matrix");
}

void matmul_deterministic_random_data()
{
    const std::vector<float> A = random_matrix(13 * 11);
    const std::vector<float> B = random_matrix(11 * 7);

    expect_all_variants_match_naive(
        A, B, 13, 11, 7, "deterministic random data");
}

} // namespace

std::vector<test::TestCase> matmul_tests()
{
    return {
        { "matmul_naive computes 1x1", matmul_1x1 },
        { "matmul_naive computes 2x2 manual example", matmul_2x2_manual },
        { "optimized matmul computes rectangular 3x5 by 5x4",
          matmul_rectangular_3x5_5x4 },
        { "optimized matmul handles non-AVX tail",
          matmul_non_avx_tail_7x10_10x9 },
        { "optimized matmul handles non-tile-aligned shapes",
          matmul_non_tile_aligned_31x17_17x23 },
        { "optimized matmul handles zero matrix", matmul_zero_matrix },
        { "optimized matmul handles identity matrix", matmul_identity_matrix },
        { "optimized matmul handles deterministic random data",
          matmul_deterministic_random_data },
    };
}
