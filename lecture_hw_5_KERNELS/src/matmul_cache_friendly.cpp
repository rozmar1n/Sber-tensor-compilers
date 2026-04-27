#include "kernels/matmul.hpp"

#include "kernels/utils.hpp"

#include <cstddef>
#include <vector>

namespace kernels {

void transpose(const float* src, float* dst, int rows, int cols)
{
    require_non_null(src, "src");
    require_non_null(dst, "dst");
    const std::size_t row_count = to_size(rows, "rows");
    const std::size_t col_count = to_size(cols, "cols");
    checked_mul_size(row_count, col_count, "transpose element count");

    for (std::size_t i = 0; i < row_count; ++i) {
        for (std::size_t j = 0; j < col_count; ++j) {
            dst[j * row_count + i] = src[i * col_count + j];
        }
    }
}

void matmul_cache_friendly(const float* A,
                           const float* B,
                           float* C,
                           int M,
                           int K,
                           int N)
{
    require_non_null(A, "A");
    require_non_null(B, "B");
    require_non_null(C, "C");
    const MatmulShape shape = checked_matmul_shape(M, K, N);

    std::vector<float> B_T(shape.b_count);
    transpose(B, B_T.data(), K, N);

    for (std::size_t i = 0; i < shape.m; ++i) {
        for (std::size_t j = 0; j < shape.n; ++j) {
            float sum = 0.0f;
            for (std::size_t k = 0; k < shape.k; ++k) {
                sum += A[i * shape.k + k] * B_T[j * shape.k + k];
            }
            C[i * shape.n + j] = sum;
        }
    }
}

} // namespace kernels
