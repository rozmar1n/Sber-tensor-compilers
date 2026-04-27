#include "kernels/matmul.hpp"

#include "kernels/utils.hpp"

#include <cstddef>

namespace kernels {

void matmul_naive(const float* A, const float* B, float* C, int M, int K, int N)
{
    require_non_null(A, "A");
    require_non_null(B, "B");
    require_non_null(C, "C");
    const MatmulShape shape = checked_matmul_shape(M, K, N);

    for (std::size_t i = 0; i < shape.m; ++i) {
        for (std::size_t j = 0; j < shape.n; ++j) {
            float sum = 0.0f;
            for (std::size_t k = 0; k < shape.k; ++k) {
                sum += A[i * shape.k + k] * B[k * shape.n + j];
            }
            C[i * shape.n + j] = sum;
        }
    }
}

} // namespace kernels
