#include "kernels/matmul.hpp"

#include "kernels/utils.hpp"

#include <vector>

namespace kernels {

void transpose(const float* src, float* dst, int rows, int cols) {
    require_non_null(src, "src");
    require_non_null(dst, "dst");
    require_positive(rows, "rows");
    require_positive(cols, "cols");

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            dst[j * rows + i] = src[i * cols + j];
        }
    }
}

void matmul_cache_friendly(const float* A, const float* B, float* C, int M, int K, int N) {
    require_non_null(A, "A");
    require_non_null(B, "B");
    require_non_null(C, "C");
    require_positive(M, "M");
    require_positive(K, "K");
    require_positive(N, "N");

    std::vector<float> B_T(static_cast<std::size_t>(N) * static_cast<std::size_t>(K));
    transpose(B, B_T.data(), K, N);

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < K; ++k) {
                sum += A[i * K + k] * B_T[j * K + k];
            }
            C[i * N + j] = sum;
        }
    }
}

}  // namespace kernels
