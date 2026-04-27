#pragma once

namespace kernels {

void matmul_naive(const float* A, const float* B, float* C, int M, int K, int N);
void transpose(const float* src, float* dst, int rows, int cols);
void matmul_cache_friendly(const float* A, const float* B, float* C, int M, int K, int N);
void matmul_tiled(const float* A, const float* B, float* C, int M, int K, int N, int tile_size);
void matmul_vectorized(const float* A, const float* B, float* C, int M, int K, int N);

}  // namespace kernels
