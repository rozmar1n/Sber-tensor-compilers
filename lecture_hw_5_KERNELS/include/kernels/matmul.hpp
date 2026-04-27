#pragma once

namespace kernels {

// All MatMul kernels use row-major float matrices:
// A[M, K] at A[i * K + k], B[K, N] at B[k * N + j],
// C[M, N] at C[i * N + j].
// Input and output buffers must be non-null, large enough, and non-overlapping.
void matmul_naive(const float* A,
                  const float* B,
                  float* C,
                  int M,
                  int K,
                  int N);

// Transposes row-major src[rows, cols] into dst[cols, rows].
// src and dst must be non-null, large enough, and non-overlapping.
void transpose(const float* src, float* dst, int rows, int cols);

void matmul_cache_friendly(const float* A,
                           const float* B,
                           float* C,
                           int M,
                           int K,
                           int N);
void matmul_tiled(const float* A,
                  const float* B,
                  float* C,
                  int M,
                  int K,
                  int N,
                  int tile_size);
void matmul_vectorized(const float* A,
                       const float* B,
                       float* C,
                       int M,
                       int K,
                       int N);

} // namespace kernels
