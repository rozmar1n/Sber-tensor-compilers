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
    require_positive(M, "M");
    require_positive(K, "K");
    require_positive(N, "N");

#if KERNELS_ENABLE_AVX2
    std::vector<float> B_T(static_cast<std::size_t>(N) *
                           static_cast<std::size_t>(K));
    transpose(B, B_T.data(), K, N);

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            __m256 acc = _mm256_setzero_ps();
            int k = 0;
            for (; k + 8 <= K; k += 8) {
                const __m256 a_vec = _mm256_loadu_ps(&A[i * K + k]);
                const __m256 b_vec = _mm256_loadu_ps(&B_T[j * K + k]);
                acc = _mm256_add_ps(acc, _mm256_mul_ps(a_vec, b_vec));
            }

            float sum = horizontal_sum(acc);
            for (; k < K; ++k) {
                sum += A[i * K + k] * B_T[j * K + k];
            }
            C[i * N + j] = sum;
        }
    }
#else
    matmul_cache_friendly(A, B, C, M, K, N);
#endif
}

} // namespace kernels
