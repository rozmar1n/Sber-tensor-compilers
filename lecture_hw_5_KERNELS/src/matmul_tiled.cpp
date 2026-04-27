#include "kernels/matmul.hpp"

#include "kernels/utils.hpp"

#include <algorithm>
#include <cstddef>

namespace kernels {

void matmul_tiled(const float* A,
                  const float* B,
                  float* C,
                  int M,
                  int K,
                  int N,
                  int tile_size)
{
    require_non_null(A, "A");
    require_non_null(B, "B");
    require_non_null(C, "C");
    const MatmulShape shape = checked_matmul_shape(M, K, N);
    const std::size_t tile = to_size(tile_size, "tile_size");

    std::fill(C, C + shape.c_count, 0.0f);

    for (std::size_t ii = 0; ii < shape.m; ii += tile) {
        for (std::size_t jj = 0; jj < shape.n; jj += tile) {
            for (std::size_t kk = 0; kk < shape.k; kk += tile) {
                const std::size_t i_end = std::min(ii + tile, shape.m);
                const std::size_t j_end = std::min(jj + tile, shape.n);
                const std::size_t k_end = std::min(kk + tile, shape.k);

                for (std::size_t i = ii; i < i_end; ++i) {
                    for (std::size_t j = jj; j < j_end; ++j) {
                        float sum = C[i * shape.n + j];
                        for (std::size_t k = kk; k < k_end; ++k) {
                            sum += A[i * shape.k + k] * B[k * shape.n + j];
                        }
                        C[i * shape.n + j] = sum;
                    }
                }
            }
        }
    }
}

} // namespace kernels
