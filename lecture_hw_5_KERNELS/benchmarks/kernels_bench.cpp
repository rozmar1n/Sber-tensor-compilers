#include "kernels/conv2d.hpp"
#include "kernels/matmul.hpp"

#include <benchmark/benchmark.h>

#include <cstddef>
#include <random>
#include <string>
#include <vector>

namespace {

volatile float g_sink = 0.0f;

std::vector<float> make_data(std::size_t count, unsigned int seed)
{
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    std::vector<float> data(count);
    for (float& value : data) {
        value = dist(rng);
    }
    return data;
}

float checksum(const std::vector<float>& values)
{
    float sum = 0.0f;
    for (float value : values) {
        sum += value;
    }
    return sum;
}

std::string matmul_label(int m, int k, int n)
{
    return std::to_string(m) + "x" + std::to_string(k) + "x" +
           std::to_string(n);
}

std::string conv_label(int batch,
                       int channels_in,
                       int height,
                       int width,
                       int channels_out,
                       int kernel_h,
                       int kernel_w)
{
    return std::to_string(batch) + "x" + std::to_string(channels_in) + "x" +
           std::to_string(height) + "x" + std::to_string(width) +
           ", kernel=" + std::to_string(channels_out) + "x" +
           std::to_string(channels_in) + "x" + std::to_string(kernel_h) + "x" +
           std::to_string(kernel_w);
}

void bench_matmul_naive(benchmark::State& state)
{
    const int size = static_cast<int>(state.range(0));
    const int m = size;
    const int k = size;
    const int n = size;

    const std::vector<float> a =
        make_data(static_cast<std::size_t>(m) * k,
                  1000U + static_cast<unsigned int>(size));
    const std::vector<float> b =
        make_data(static_cast<std::size_t>(k) * n,
                  2000U + static_cast<unsigned int>(size));
    std::vector<float> c(static_cast<std::size_t>(m) * n, 0.0f);

    for (auto _ : state) {
        benchmark::DoNotOptimize(a.data());
        benchmark::DoNotOptimize(b.data());
        kernels::matmul_naive(a.data(), b.data(), c.data(), m, k, n);
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }

    g_sink += checksum(c);
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(m) * n);
    state.SetLabel(matmul_label(m, k, n));
}

void bench_matmul_cache_friendly(benchmark::State& state)
{
    const int size = static_cast<int>(state.range(0));
    const int m = size;
    const int k = size;
    const int n = size;

    const std::vector<float> a =
        make_data(static_cast<std::size_t>(m) * k,
                  1000U + static_cast<unsigned int>(size));
    const std::vector<float> b =
        make_data(static_cast<std::size_t>(k) * n,
                  2000U + static_cast<unsigned int>(size));
    std::vector<float> c(static_cast<std::size_t>(m) * n, 0.0f);

    for (auto _ : state) {
        benchmark::DoNotOptimize(a.data());
        benchmark::DoNotOptimize(b.data());
        kernels::matmul_cache_friendly(a.data(), b.data(), c.data(), m, k, n);
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }

    g_sink += checksum(c);
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(m) * n);
    state.SetLabel(matmul_label(m, k, n));
}

void bench_matmul_tiled(benchmark::State& state)
{
    const int size = static_cast<int>(state.range(0));
    const int tile = static_cast<int>(state.range(1));
    const int m = size;
    const int k = size;
    const int n = size;

    const std::vector<float> a =
        make_data(static_cast<std::size_t>(m) * k,
                  1000U + static_cast<unsigned int>(size));
    const std::vector<float> b =
        make_data(static_cast<std::size_t>(k) * n,
                  2000U + static_cast<unsigned int>(size));
    std::vector<float> c(static_cast<std::size_t>(m) * n, 0.0f);

    for (auto _ : state) {
        benchmark::DoNotOptimize(a.data());
        benchmark::DoNotOptimize(b.data());
        kernels::matmul_tiled(a.data(), b.data(), c.data(), m, k, n, tile);
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }

    g_sink += checksum(c);
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(m) * n);
    state.SetLabel(matmul_label(m, k, n) + ", tile=" + std::to_string(tile));
}

void bench_matmul_vectorized(benchmark::State& state)
{
    const int size = static_cast<int>(state.range(0));
    const int m = size;
    const int k = size;
    const int n = size;

    const std::vector<float> a =
        make_data(static_cast<std::size_t>(m) * k,
                  1000U + static_cast<unsigned int>(size));
    const std::vector<float> b =
        make_data(static_cast<std::size_t>(k) * n,
                  2000U + static_cast<unsigned int>(size));
    std::vector<float> c(static_cast<std::size_t>(m) * n, 0.0f);

    for (auto _ : state) {
        benchmark::DoNotOptimize(a.data());
        benchmark::DoNotOptimize(b.data());
        kernels::matmul_vectorized(a.data(), b.data(), c.data(), m, k, n);
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }

    g_sink += checksum(c);
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(m) * n);
    state.SetLabel(matmul_label(m, k, n));
}

void bench_conv2d_naive(benchmark::State& state)
{
    const int batch = static_cast<int>(state.range(0));
    const int channels_in = static_cast<int>(state.range(1));
    const int height = static_cast<int>(state.range(2));
    const int width = static_cast<int>(state.range(3));
    const int channels_out = static_cast<int>(state.range(4));
    const int kernel_h = static_cast<int>(state.range(5));
    const int kernel_w = static_cast<int>(state.range(6));

    const int height_out = kernels::conv2d_output_size(height, kernel_h);
    const int width_out = kernels::conv2d_output_size(width, kernel_w);

    const std::size_t input_size =
        static_cast<std::size_t>(batch) * channels_in * height * width;
    const std::size_t kernel_size = static_cast<std::size_t>(channels_out) *
                                    channels_in * kernel_h * kernel_w;
    const std::size_t output_size =
        static_cast<std::size_t>(batch) * channels_out * height_out * width_out;

    const std::vector<float> input = make_data(
        input_size, 3000U + static_cast<unsigned int>(height + channels_in));
    const std::vector<float> kernel = make_data(
        kernel_size, 4000U + static_cast<unsigned int>(height + channels_out));
    std::vector<float> output(output_size, 0.0f);

    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        benchmark::DoNotOptimize(kernel.data());
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
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }

    g_sink += checksum(output);
    state.SetItemsProcessed(state.iterations() *
                            static_cast<int64_t>(output_size));
    state.SetLabel(conv_label(
        batch, channels_in, height, width, channels_out, kernel_h, kernel_w));
}

void bench_conv2d_im2col(benchmark::State& state)
{
    const int batch = static_cast<int>(state.range(0));
    const int channels_in = static_cast<int>(state.range(1));
    const int height = static_cast<int>(state.range(2));
    const int width = static_cast<int>(state.range(3));
    const int channels_out = static_cast<int>(state.range(4));
    const int kernel_h = static_cast<int>(state.range(5));
    const int kernel_w = static_cast<int>(state.range(6));

    const int height_out = kernels::conv2d_output_size(height, kernel_h);
    const int width_out = kernels::conv2d_output_size(width, kernel_w);

    const std::size_t input_size =
        static_cast<std::size_t>(batch) * channels_in * height * width;
    const std::size_t kernel_size = static_cast<std::size_t>(channels_out) *
                                    channels_in * kernel_h * kernel_w;
    const std::size_t output_size =
        static_cast<std::size_t>(batch) * channels_out * height_out * width_out;

    const std::vector<float> input = make_data(
        input_size, 3000U + static_cast<unsigned int>(height + channels_in));
    const std::vector<float> kernel = make_data(
        kernel_size, 4000U + static_cast<unsigned int>(height + channels_out));
    std::vector<float> output(output_size, 0.0f);

    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        benchmark::DoNotOptimize(kernel.data());
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
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }

    g_sink += checksum(output);
    state.SetItemsProcessed(state.iterations() *
                            static_cast<int64_t>(output_size));
    state.SetLabel(conv_label(
        batch, channels_in, height, width, channels_out, kernel_h, kernel_w));
}

} // namespace

BENCHMARK(bench_matmul_naive)
    ->ArgName("size")
    ->Arg(64)
    ->Arg(128)
    ->Arg(256)
    ->Arg(512);

BENCHMARK(bench_matmul_cache_friendly)
    ->ArgName("size")
    ->Arg(64)
    ->Arg(128)
    ->Arg(256)
    ->Arg(512);

BENCHMARK(bench_matmul_tiled)
    ->ArgNames({ "size", "tile" })
    ->Args({ 64, 32 })
    ->Args({ 128, 32 })
    ->Args({ 256, 32 })
    ->Args({ 512, 32 });

BENCHMARK(bench_matmul_vectorized)
    ->ArgName("size")
    ->Arg(64)
    ->Arg(128)
    ->Arg(256)
    ->Arg(512);

BENCHMARK(bench_conv2d_naive)
    ->ArgNames({ "batch",
                 "channels_in",
                 "height",
                 "width",
                 "channels_out",
                 "kernel_h",
                 "kernel_w" })
    ->Args({ 1, 3, 32, 32, 8, 3, 3 })
    ->Args({ 1, 3, 64, 64, 16, 3, 3 })
    ->Args({ 1, 16, 64, 64, 32, 3, 3 });

BENCHMARK(bench_conv2d_im2col)
    ->ArgNames({ "batch",
                 "channels_in",
                 "height",
                 "width",
                 "channels_out",
                 "kernel_h",
                 "kernel_w" })
    ->Args({ 1, 3, 32, 32, 8, 3, 3 })
    ->Args({ 1, 3, 64, 64, 16, 3, 3 })
    ->Args({ 1, 16, 64, 64, 32, 3, 3 });

BENCHMARK_MAIN();
