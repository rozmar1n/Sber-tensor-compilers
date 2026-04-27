#include "kernels/conv2d.hpp"
#include "test_utils.hpp"

#include <limits>
#include <random>
#include <string>
#include <vector>

namespace {

constexpr float kEps = 1e-4f;

std::vector<float> sequential_values(int size,
                                     float scale = 1.0f,
                                     float offset = 0.0f)
{
    std::vector<float> values(size);
    for (int i = 0; i < size; ++i) {
        values[i] = offset + scale * static_cast<float>(i + 1);
    }
    return values;
}

std::vector<float> random_values(int size)
{
    std::mt19937 rng(24680);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    std::vector<float> values(size);
    for (float& value : values) {
        value = dist(rng);
    }
    return values;
}

int output_elements(int batch,
                    int channels_out,
                    int height,
                    int width,
                    int kernel_h,
                    int kernel_w)
{
    const int h_out = kernels::conv2d_output_size(height, kernel_h);
    const int w_out = kernels::conv2d_output_size(width, kernel_w);
    return batch * channels_out * h_out * w_out;
}

void expect_im2col_matches_naive(const std::vector<float>& input,
                                 const std::vector<float>& kernel,
                                 int batch,
                                 int channels_in,
                                 int height,
                                 int width,
                                 int channels_out,
                                 int kernel_h,
                                 int kernel_w,
                                 const char* case_name)
{
    const int out_size =
        output_elements(batch, channels_out, height, width, kernel_h, kernel_w);
    std::vector<float> expected(out_size, 0.0f);
    std::vector<float> actual(out_size, -1.0f);

    kernels::conv2d_naive(input.data(),
                          kernel.data(),
                          expected.data(),
                          batch,
                          channels_in,
                          height,
                          width,
                          channels_out,
                          kernel_h,
                          kernel_w);
    kernels::conv2d_im2col(input.data(),
                           kernel.data(),
                           actual.data(),
                           batch,
                           channels_in,
                           height,
                           width,
                           channels_out,
                           kernel_h,
                           kernel_w);

    test::expect_vector_near(actual, expected, kEps, case_name);
}

void conv2d_manual_3x3_2x2()
{
    const std::vector<float> input{
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f,
    };
    const std::vector<float> kernel{
        1.0f,
        0.0f,
        0.0f,
        -1.0f,
    };
    std::vector<float> output(4, 0.0f);

    kernels::conv2d_naive(
        input.data(), kernel.data(), output.data(), 1, 1, 3, 3, 1, 2, 2);

    test::expect_vector_near(
        output, { -4.0f, -4.0f, -4.0f, -4.0f }, kEps, "manual 3x3 2x2 conv");
    expect_im2col_matches_naive(
        input, kernel, 1, 1, 3, 3, 1, 2, 2, "manual 3x3 2x2 im2col conv");
}

void conv2d_single_channel_5x5_3x3()
{
    const std::vector<float> input =
        sequential_values(1 * 1 * 5 * 5, 0.25f, -2.0f);
    const std::vector<float> kernel =
        sequential_values(1 * 1 * 3 * 3, -0.125f, 1.0f);

    expect_im2col_matches_naive(
        input, kernel, 1, 1, 5, 5, 1, 3, 3, "single-channel 5x5 3x3");
}

void conv2d_multi_channel_multi_output()
{
    const std::vector<float> input =
        sequential_values(1 * 3 * 8 * 8, 0.01f, -1.0f);
    const std::vector<float> kernel =
        sequential_values(4 * 3 * 3 * 3, -0.015f, 0.75f);

    expect_im2col_matches_naive(
        input, kernel, 1, 3, 8, 8, 4, 3, 3, "N=1 C_in=3 C_out=4 8x8 3x3");
}

void conv2d_multi_batch_multi_channel()
{
    const std::vector<float> input =
        sequential_values(2 * 3 * 8 * 8, 0.02f, -3.0f);
    const std::vector<float> kernel =
        sequential_values(5 * 3 * 3 * 3, 0.01f, -0.5f);

    expect_im2col_matches_naive(
        input, kernel, 2, 3, 8, 8, 5, 3, 3, "N=2 C_in=3 C_out=5 8x8 3x3");
}

void conv2d_kernel_1x1()
{
    const std::vector<float> input =
        sequential_values(1 * 3 * 4 * 4, 0.1f, -2.0f);
    const std::vector<float> kernel =
        sequential_values(2 * 3 * 1 * 1, 0.2f, -0.3f);

    expect_im2col_matches_naive(input, kernel, 1, 3, 4, 4, 2, 1, 1, "1x1 conv");
}

void conv2d_deterministic_random_data()
{
    const std::vector<float> input = random_values(2 * 3 * 8 * 8);
    const std::vector<float> kernel = random_values(5 * 3 * 3 * 3);

    expect_im2col_matches_naive(
        input, kernel, 2, 3, 8, 8, 5, 3, 3, "deterministic random conv");
}

void im2col_small_direct()
{
    const std::vector<float> input{
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f,
    };
    std::vector<float> col(4 * 4, 0.0f);

    kernels::im2col(input.data(), col.data(), 1, 1, 3, 3, 2, 2);

    test::expect_vector_near(col,
                             {
                                 1.0f,
                                 2.0f,
                                 4.0f,
                                 5.0f,
                                 2.0f,
                                 3.0f,
                                 5.0f,
                                 6.0f,
                                 4.0f,
                                 5.0f,
                                 7.0f,
                                 8.0f,
                                 5.0f,
                                 6.0f,
                                 8.0f,
                                 9.0f,
                             },
                             kEps,
                             "direct im2col");
}

void reshape_kernel_small_direct()
{
    const std::vector<float> kernel{
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f,
    };
    std::vector<float> kernel_matrix(4 * 2, 0.0f);

    kernels::reshape_kernel_for_im2col(
        kernel.data(), kernel_matrix.data(), 2, 1, 2, 2);

    test::expect_vector_near(kernel_matrix,
                             {
                                 1.0f,
                                 5.0f,
                                 2.0f,
                                 6.0f,
                                 3.0f,
                                 7.0f,
                                 4.0f,
                                 8.0f,
                             },
                             kEps,
                             "kernel reshape");
}

void im2col_multi_channel_direct()
{
    const std::vector<float> input{
        1.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,

        101.0f, 102.0f, 103.0f, 104.0f, 105.0f, 106.0f, 107.0f, 108.0f, 109.0f,
    };
    std::vector<float> col(4 * 8, 0.0f);

    kernels::im2col(input.data(), col.data(), 1, 2, 3, 3, 2, 2);

    test::expect_vector_near(
        col,
        {
            1.0f, 2.0f, 4.0f, 5.0f, 101.0f, 102.0f, 104.0f, 105.0f,
            2.0f, 3.0f, 5.0f, 6.0f, 102.0f, 103.0f, 105.0f, 106.0f,
            4.0f, 5.0f, 7.0f, 8.0f, 104.0f, 105.0f, 107.0f, 108.0f,
            5.0f, 6.0f, 8.0f, 9.0f, 105.0f, 106.0f, 108.0f, 109.0f,
        },
        kEps,
        "multi-channel direct im2col");
}

void reshape_kernel_multi_channel_direct()
{
    const std::vector<float> kernel{
        1.0f,   2.0f,   3.0f,   4.0f,   11.0f,  12.0f,  13.0f,  14.0f,

        101.0f, 102.0f, 103.0f, 104.0f, 111.0f, 112.0f, 113.0f, 114.0f,
    };
    std::vector<float> kernel_matrix(8 * 2, 0.0f);

    kernels::reshape_kernel_for_im2col(
        kernel.data(), kernel_matrix.data(), 2, 2, 2, 2);

    test::expect_vector_near(kernel_matrix,
                             {
                                 1.0f,
                                 101.0f,
                                 2.0f,
                                 102.0f,
                                 3.0f,
                                 103.0f,
                                 4.0f,
                                 104.0f,
                                 11.0f,
                                 111.0f,
                                 12.0f,
                                 112.0f,
                                 13.0f,
                                 113.0f,
                                 14.0f,
                                 114.0f,
                             },
                             kEps,
                             "multi-channel kernel reshape");
}

void conv2d_im2col_rejects_oversized_matmul_shape()
{
    const std::vector<float> input{ 1.0f };
    const std::vector<float> kernel{ 1.0f };
    std::vector<float> output(1, 0.0f);

    bool threw = false;
    try {
        kernels::conv2d_im2col(input.data(),
                               kernel.data(),
                               output.data(),
                               std::numeric_limits<int>::max(),
                               1,
                               2,
                               2,
                               1,
                               1,
                               1);
    } catch (const std::overflow_error&) {
        threw = true;
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    if (!threw) {
        throw std::runtime_error(
            "conv2d_im2col did not reject oversized matmul shape");
    }
}

} // namespace

std::vector<test::TestCase> conv2d_tests()
{
    return {
        { "conv2d_naive computes manual 3x3 input with 2x2 kernel",
          conv2d_manual_3x3_2x2 },
        { "conv2d_im2col matches naive for 5x5 single-channel 3x3",
          conv2d_single_channel_5x5_3x3 },
        { "conv2d_im2col matches naive for multi-channel multi-output",
          conv2d_multi_channel_multi_output },
        { "conv2d_im2col matches naive for multi-batch multi-channel",
          conv2d_multi_batch_multi_channel },
        { "conv2d_im2col matches naive for 1x1 kernel", conv2d_kernel_1x1 },
        { "conv2d_im2col matches naive for deterministic random data",
          conv2d_deterministic_random_data },
        { "im2col expands a small 3x3 input directly", im2col_small_direct },
        { "reshape_kernel_for_im2col maps kernel to GEMM layout",
          reshape_kernel_small_direct },
        { "im2col preserves multi-channel column order",
          im2col_multi_channel_direct },
        { "reshape_kernel_for_im2col preserves multi-channel GEMM order",
          reshape_kernel_multi_channel_direct },
        { "conv2d_im2col rejects oversized matmul shapes",
          conv2d_im2col_rejects_oversized_matmul_shape },
    };
}
