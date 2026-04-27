#include "kernels/matmul.hpp"

#include "kernels/utils.hpp"

namespace kernels {

void matmul_naive(const float* A, const float* B, float* C, int M, int K, int N) {
    require_non_null(A, "A");
    require_non_null(B, "B");
    require_non_null(C, "C");
    require_positive(M, "M");
    require_positive(K, "K");
    require_positive(N, "N");

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < K; ++k) {
                sum += A[i * K + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

}  // namespace kernels
