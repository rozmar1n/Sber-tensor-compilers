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
    require_positive(M, "M");
    require_positive(K, "K");
    require_positive(N, "N");
    require_positive(tile_size, "tile_size");

    std::fill(
        C, C + static_cast<std::size_t>(M) * static_cast<std::size_t>(N), 0.0f);

    for (int ii = 0; ii < M; ii += tile_size) {
        for (int jj = 0; jj < N; jj += tile_size) {
            for (int kk = 0; kk < K; kk += tile_size) {
                const int i_end = std::min(ii + tile_size, M);
                const int j_end = std::min(jj + tile_size, N);
                const int k_end = std::min(kk + tile_size, K);

                for (int i = ii; i < i_end; ++i) {
                    for (int j = jj; j < j_end; ++j) {
                        float sum = C[i * N + j];
                        for (int k = kk; k < k_end; ++k) {
                            sum += A[i * K + k] * B[k * N + j];
                        }
                        C[i * N + j] = sum;
                    }
                }
            }
        }
    }
}

} // namespace kernels
