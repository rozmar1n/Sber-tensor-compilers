#include "kernels/conv2d.hpp"
#include "test_utils.hpp"

#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr float kEps = 1e-4f;

struct ConvShape
{
    int batch;
    int channels_in;
    int height;
    int width;
    int channels_out;
    int kernel_h;
    int kernel_w;
};

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

std::vector<float> random_values(int size, unsigned int seed)
{
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    std::vector<float> values(size);
    for (float& value : values) {
        value = dist(rng);
    }
    return values;
}

int input_elements(const ConvShape& shape)
{
    return shape.batch * shape.channels_in * shape.height * shape.width;
}

int kernel_elements(const ConvShape& shape)
{
    return shape.channels_out * shape.channels_in * shape.kernel_h *
           shape.kernel_w;
}

int output_elements(const ConvShape& shape)
{
    const int h_out = kernels::conv2d_output_size(shape.height, shape.kernel_h);
    const int w_out = kernels::conv2d_output_size(shape.width, shape.kernel_w);
    return shape.batch * shape.channels_out * h_out * w_out;
}

void expect_im2col_matches_naive(const std::vector<float>& input,
                                 const std::vector<float>& kernel,
                                 const ConvShape& shape,
                                 const char* case_name)
{
    const int out_size = output_elements(shape);
    std::vector<float> expected(out_size, 0.0f);
    std::vector<float> actual(out_size, -1.0f);

    kernels::conv2d_naive(input.data(),
                          kernel.data(),
                          expected.data(),
                          shape.batch,
                          shape.channels_in,
                          shape.height,
                          shape.width,
                          shape.channels_out,
                          shape.kernel_h,
                          shape.kernel_w);
    kernels::conv2d_im2col(input.data(),
                           kernel.data(),
                           actual.data(),
                           shape.batch,
                           shape.channels_in,
                           shape.height,
                           shape.width,
                           shape.channels_out,
                           shape.kernel_h,
                           shape.kernel_w);

    test::expect_vector_near(actual, expected, kEps, case_name);
}

void expect_conv2d_matches_expected(const std::vector<float>& input,
                                    const std::vector<float>& kernel,
                                    const std::vector<float>& expected,
                                    const ConvShape& shape,
                                    const char* case_name)
{
    std::vector<float> naive(output_elements(shape), 0.0f);
    std::vector<float> im2col(output_elements(shape), -1.0f);

    kernels::conv2d_naive(input.data(),
                          kernel.data(),
                          naive.data(),
                          shape.batch,
                          shape.channels_in,
                          shape.height,
                          shape.width,
                          shape.channels_out,
                          shape.kernel_h,
                          shape.kernel_w);
    kernels::conv2d_im2col(input.data(),
                           kernel.data(),
                           im2col.data(),
                           shape.batch,
                           shape.channels_in,
                           shape.height,
                           shape.width,
                           shape.channels_out,
                           shape.kernel_h,
                           shape.kernel_w);

    test::expect_vector_near(naive, expected, kEps, case_name);
    test::expect_vector_near(im2col, expected, kEps, case_name);
}

void conv2d_manual_3x3_2x2()
{
    constexpr ConvShape shape{ 1, 1, 3, 3, 1, 2, 2 };
    const std::vector<float> input{
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f,
    };
    const std::vector<float> kernel{
        1.0f,
        0.0f,
        0.0f,
        -1.0f,
    };

    expect_conv2d_matches_expected(input,
                                   kernel,
                                   { -4.0f, -4.0f, -4.0f, -4.0f },
                                   shape,
                                   "manual 3x3 2x2 conv");
}

void conv2d_multi_channel_1x1_expected()
{
    constexpr ConvShape shape{ 1, 2, 2, 2, 2, 1, 1 };
    const std::vector<float> input{
        1.0f, 2.0f, 3.0f, 4.0f, 10.0f, 20.0f, 30.0f, 40.0f,
    };
    const std::vector<float> kernel{
        1.0f,
        0.5f,
        -1.0f,
        2.0f,
    };

    expect_conv2d_matches_expected(
        input,
        kernel,
        { 6.0f, 12.0f, 18.0f, 24.0f, 19.0f, 38.0f, 57.0f, 76.0f },
        shape,
        "multi-channel 1x1 conv");
}

void conv2d_single_channel_5x5_3x3()
{
    constexpr ConvShape shape{ 1, 1, 5, 5, 1, 3, 3 };
    const std::vector<float> input =
        sequential_values(input_elements(shape), 0.25f, -2.0f);
    const std::vector<float> kernel =
        sequential_values(kernel_elements(shape), -0.125f, 1.0f);

    expect_im2col_matches_naive(input, kernel, shape, "single-channel 5x5 3x3");
}

void conv2d_multi_channel_multi_output()
{
    constexpr ConvShape shape{ 1, 3, 8, 8, 4, 3, 3 };
    const std::vector<float> input =
        sequential_values(input_elements(shape), 0.01f, -1.0f);
    const std::vector<float> kernel =
        sequential_values(kernel_elements(shape), -0.015f, 0.75f);

    expect_im2col_matches_naive(
        input, kernel, shape, "N=1 C_in=3 C_out=4 8x8 3x3");
}

void conv2d_multi_batch_multi_channel()
{
    constexpr ConvShape shape{ 2, 3, 8, 8, 5, 3, 3 };
    const std::vector<float> input =
        sequential_values(input_elements(shape), 0.02f, -3.0f);
    const std::vector<float> kernel =
        sequential_values(kernel_elements(shape), 0.01f, -0.5f);

    expect_im2col_matches_naive(
        input, kernel, shape, "N=2 C_in=3 C_out=5 8x8 3x3");
}

void conv2d_kernel_1x1()
{
    constexpr ConvShape shape{ 1, 3, 4, 4, 2, 1, 1 };
    const std::vector<float> input =
        sequential_values(input_elements(shape), 0.1f, -2.0f);
    const std::vector<float> kernel =
        sequential_values(kernel_elements(shape), 0.2f, -0.3f);

    expect_im2col_matches_naive(input, kernel, shape, "1x1 conv");
}

void conv2d_deterministic_random_data()
{
    constexpr ConvShape shape{ 2, 3, 8, 8, 5, 3, 3 };
    const std::vector<float> input =
        random_values(input_elements(shape), 24680);
    const std::vector<float> kernel =
        random_values(kernel_elements(shape), 86420);

    expect_im2col_matches_naive(
        input, kernel, shape, "deterministic random conv");
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

    EXPECT_THROW(kernels::conv2d_im2col(input.data(),
                                        kernel.data(),
                                        output.data(),
                                        std::numeric_limits<int>::max(),
                                        1,
                                        2,
                                        2,
                                        1,
                                        1,
                                        1),
                 std::exception);
}

} // namespace

TEST(Conv2DTest, NaiveComputesManual3x3InputWith2x2Kernel)
{
    conv2d_manual_3x3_2x2();
}

TEST(Conv2DTest, AllKernelsComputeMultiChannel1x1ExpectedOutput)
{
    conv2d_multi_channel_1x1_expected();
}

TEST(Conv2DTest, Im2ColMatchesNaiveFor5x5SingleChannel3x3)
{
    conv2d_single_channel_5x5_3x3();
}

TEST(Conv2DTest, Im2ColMatchesNaiveForMultiChannelMultiOutput)
{
    conv2d_multi_channel_multi_output();
}

TEST(Conv2DTest, Im2ColMatchesNaiveForMultiBatchMultiChannel)
{
    conv2d_multi_batch_multi_channel();
}

TEST(Conv2DTest, Im2ColMatchesNaiveFor1x1Kernel)
{
    conv2d_kernel_1x1();
}

TEST(Conv2DTest, Im2ColMatchesNaiveForDeterministicRandomData)
{
    conv2d_deterministic_random_data();
}

TEST(Conv2DTest, Im2ColExpandsSmall3x3InputDirectly)
{
    im2col_small_direct();
}

TEST(Conv2DTest, ReshapeKernelMapsKernelToGemmLayout)
{
    reshape_kernel_small_direct();
}

TEST(Conv2DTest, Im2ColPreservesMultiChannelColumnOrder)
{
    im2col_multi_channel_direct();
}

TEST(Conv2DTest, ReshapeKernelPreservesMultiChannelGemmOrder)
{
    reshape_kernel_multi_channel_direct();
}

TEST(Conv2DTest, Im2ColRejectsOversizedMatmulShapes)
{
    conv2d_im2col_rejects_oversized_matmul_shape();
}
