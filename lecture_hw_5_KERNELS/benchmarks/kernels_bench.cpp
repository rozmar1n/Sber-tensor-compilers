#include "kernels/conv2d.hpp"
#include "kernels/matmul.hpp"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

volatile float g_sink = 0.0f;

std::vector<float> make_data(std::size_t count, unsigned int seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    std::vector<float> data(count);
    for (float& value : data) {
        value = dist(rng);
    }
    return data;
}

float checksum(const std::vector<float>& values) {
    float sum = 0.0f;
    for (float value : values) {
        sum += value;
    }
    return sum;
}

template <typename Kernel>
double benchmark_kernel(Kernel&& kernel, int repeats) {
    kernel();

    const auto start = Clock::now();
    for (int repeat = 0; repeat < repeats; ++repeat) {
        kernel();
    }
    const auto end = Clock::now();

    const std::chrono::duration<double, std::milli> elapsed = end - start;
    return elapsed.count() / static_cast<double>(repeats);
}

int repeats_for_matmul(int size) {
    if (size <= 64) {
        return 10;
    }
    if (size <= 128) {
        return 5;
    }
    return 1;
}

int repeats_for_conv(int height, int channels_in, int channels_out) {
    if (height <= 32) {
        return 5;
    }
    if (channels_in <= 3 && channels_out <= 16) {
        return 3;
    }
    return 1;
}

void print_matmul_row(const char* variant, int m, int k, int n, double time_ms) {
    std::cout << "matmul," << variant << ',' << m << ',' << k << ',' << n << ",,,"
              << time_ms << '\n';
}

void print_conv_row(const char* variant,
                    const std::string& input_shape,
                    const std::string& kernel_shape,
                    double time_ms) {
    std::cout << "conv," << variant << ",,,," << input_shape << ',' << kernel_shape << ','
              << time_ms << '\n';
}

void run_matmul_benchmarks() {
    const int sizes[] = {64, 128, 256, 512};

    for (int size : sizes) {
        const int m = size;
        const int k = size;
        const int n = size;
        const int repeats = repeats_for_matmul(size);

        const std::vector<float> a = make_data(static_cast<std::size_t>(m) * k,
                                               1000U + static_cast<unsigned int>(size));
        const std::vector<float> b = make_data(static_cast<std::size_t>(k) * n,
                                               2000U + static_cast<unsigned int>(size));
        std::vector<float> c(static_cast<std::size_t>(m) * n, 0.0f);

        double time_ms = benchmark_kernel(
            [&] { kernels::matmul_naive(a.data(), b.data(), c.data(), m, k, n); }, repeats);
        g_sink += checksum(c);
        print_matmul_row("naive", m, k, n, time_ms);

        time_ms = benchmark_kernel(
            [&] { kernels::matmul_cache_friendly(a.data(), b.data(), c.data(), m, k, n); },
            repeats);
        g_sink += checksum(c);
        print_matmul_row("cache_friendly", m, k, n, time_ms);

        time_ms = benchmark_kernel(
            [&] { kernels::matmul_tiled(a.data(), b.data(), c.data(), m, k, n, 32); },
            repeats);
        g_sink += checksum(c);
        print_matmul_row("tiled", m, k, n, time_ms);

        time_ms = benchmark_kernel(
            [&] { kernels::matmul_vectorized(a.data(), b.data(), c.data(), m, k, n); },
            repeats);
        g_sink += checksum(c);
        print_matmul_row("vectorized", m, k, n, time_ms);
    }
}

void run_conv_benchmark(int batch,
                        int channels_in,
                        int height,
                        int width,
                        int channels_out,
                        int kernel_h,
                        int kernel_w) {
    const int height_out = kernels::conv2d_output_size(height, kernel_h);
    const int width_out = kernels::conv2d_output_size(width, kernel_w);
    const int repeats = repeats_for_conv(height, channels_in, channels_out);

    const std::size_t input_size =
        static_cast<std::size_t>(batch) * channels_in * height * width;
    const std::size_t kernel_size =
        static_cast<std::size_t>(channels_out) * channels_in * kernel_h * kernel_w;
    const std::size_t output_size =
        static_cast<std::size_t>(batch) * channels_out * height_out * width_out;

    const std::vector<float> input =
        make_data(input_size, 3000U + static_cast<unsigned int>(height + channels_in));
    const std::vector<float> kernel =
        make_data(kernel_size, 4000U + static_cast<unsigned int>(height + channels_out));
    std::vector<float> output(output_size, 0.0f);

    const std::string input_shape = std::to_string(batch) + 'x' +
                                    std::to_string(channels_in) + 'x' +
                                    std::to_string(height) + 'x' + std::to_string(width);
    const std::string kernel_shape = std::to_string(channels_out) + 'x' +
                                     std::to_string(channels_in) + 'x' +
                                     std::to_string(kernel_h) + 'x' +
                                     std::to_string(kernel_w);

    double time_ms = benchmark_kernel(
        [&] {
            kernels::conv2d_naive(input.data(),
                                  kernel.data(),
                                  output.data(),
                                  batch,
                                  channels_in,
                                  height,
                                  width,
                                  channels_out,
                                  kernel_h,
                                  kernel_w);
        },
        repeats);
    g_sink += checksum(output);
    print_conv_row("naive", input_shape, kernel_shape, time_ms);

    time_ms = benchmark_kernel(
        [&] {
            kernels::conv2d_im2col(input.data(),
                                   kernel.data(),
                                   output.data(),
                                   batch,
                                   channels_in,
                                   height,
                                   width,
                                   channels_out,
                                   kernel_h,
                                   kernel_w);
        },
        repeats);
    g_sink += checksum(output);
    print_conv_row("im2col", input_shape, kernel_shape, time_ms);
}

void run_conv_benchmarks() {
    run_conv_benchmark(1, 3, 32, 32, 8, 3, 3);
    run_conv_benchmark(1, 3, 64, 64, 16, 3, 3);
    run_conv_benchmark(1, 16, 64, 64, 32, 3, 3);
}

}  // namespace

int main() {
    std::cout << "operation,variant,M,K,N,input_shape,kernel_shape,time_ms\n";
    run_matmul_benchmarks();
    run_conv_benchmarks();

    if (g_sink == 1234567.0f) {
        std::cerr << "unreachable sink value\n";
    }

    return 0;
}
