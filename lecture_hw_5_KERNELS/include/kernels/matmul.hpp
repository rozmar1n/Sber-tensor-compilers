#pragma once

namespace kernels {

void matmul_naive(const float* A, const float* B, float* C, int M, int K, int N);

}  // namespace kernels
