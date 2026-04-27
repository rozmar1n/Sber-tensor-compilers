#include "kernels/matmul.hpp"

#include <chrono>
#include <iostream>
#include <vector>

int main() {
    constexpr int M = 64;
    constexpr int K = 64;
    constexpr int N = 64;
    constexpr int repeats = 5;

    std::vector<float> A(M * K, 1.0f);
    std::vector<float> B(K * N, 2.0f);
    std::vector<float> C(M * N, 0.0f);

    kernels::matmul_naive(A.data(), B.data(), C.data(), M, K, N);

    const auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < repeats; ++i) {
        kernels::matmul_naive(A.data(), B.data(), C.data(), M, K, N);
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const auto elapsed =
        std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start);

    std::cout << "operation,variant,M,K,N,input_shape,kernel_shape,time_ms\n";
    std::cout << "matmul,naive," << M << ',' << K << ',' << N << ",,,"
              << elapsed.count() / repeats << '\n';

    return 0;
}
