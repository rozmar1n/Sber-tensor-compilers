#include "kernels/matmul.hpp"

#include "kernels/utils.hpp"

#include <cstddef>
#include <vector>

#if KERNELS_ENABLE_AVX2
#include <immintrin.h>
#endif

namespace kernels {

namespace {

#if KERNELS_ENABLE_AVX2
float horizontal_sum(__m256 values)
{
    alignas(32) float lanes[8];
    _mm256_storeu_ps(lanes, values);

    float sum = 0.0f;
    for (float lane : lanes) {
        sum += lane;
    }
    return sum;
}
#endif

} // namespace

void matmul_vectorized(const float* A,
                       const float* B,
                       float* C,
                       int M,
                       int K,
                       int N)
{
    require_non_null(A, "A");
    require_non_null(B, "B");
    require_non_null(C, "C");

#if KERNELS_ENABLE_AVX2
    const MatmulShape shape = checked_matmul_shape(M, K, N);
    std::vector<float> B_T(shape.b_count);
    transpose(B, B_T.data(), K, N);

    for (std::size_t i = 0; i < shape.m; ++i) {
        for (std::size_t j = 0; j < shape.n; ++j) {
            __m256 acc = _mm256_setzero_ps();
            std::size_t k = 0;
            for (; k + 8 <= shape.k; k += 8) {
                const __m256 a_vec = _mm256_loadu_ps(&A[i * shape.k + k]);
                const __m256 b_vec = _mm256_loadu_ps(&B_T[j * shape.k + k]);
                acc = _mm256_add_ps(acc, _mm256_mul_ps(a_vec, b_vec));
            }

            float sum = horizontal_sum(acc);
            for (; k < shape.k; ++k) {
                sum += A[i * shape.k + k] * B_T[j * shape.k + k];
            }
            C[i * shape.n + j] = sum;
        }
    }
#else
    // Non-AVX2 builds keep the same API and use the scalar transposed kernel.
    matmul_cache_friendly(A, B, C, M, K, N);
#endif
}

} // namespace kernels
