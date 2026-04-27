#include "kernels/conv2d.hpp"

#include "kernels/matmul.hpp"
#include "kernels/utils.hpp"

#include <cstddef>
#include <vector>

namespace kernels {

void conv2d_im2col(const float* input,
                   const float* kernel,
                   float* output,
                   int batch,
                   int channels_in,
                   int height,
                   int width,
                   int channels_out,
                   int kernel_h,
                   int kernel_w)
{
    require_non_null(input, "input");
    require_non_null(kernel, "kernel");
    require_non_null(output, "output");
    require_positive(batch, "batch");
    require_positive(channels_in, "channels_in");
    require_positive(height, "height");
    require_positive(width, "width");
    require_positive(channels_out, "channels_out");
    require_positive(kernel_h, "kernel_h");
    require_positive(kernel_w, "kernel_w");

    const int height_out = conv2d_output_size(height, kernel_h);
    const int width_out = conv2d_output_size(width, kernel_w);
    const std::size_t batch_size = to_size(batch, "batch");
    const std::size_t channels_in_size = to_size(channels_in, "channels_in");
    const std::size_t height_size = to_size(height, "height");
    const std::size_t width_size = to_size(width, "width");
    const std::size_t height_out_size = to_size(height_out, "height_out");
    const std::size_t width_out_size = to_size(width_out, "width_out");
    const std::size_t channels_out_size = to_size(channels_out, "channels_out");
    const std::size_t kernel_h_size = to_size(kernel_h, "kernel_h");
    const std::size_t kernel_w_size = to_size(kernel_w, "kernel_w");

    const std::size_t patch_count = checked_mul_size(
        checked_mul_size(batch_size, height_out_size, "patch count"),
        width_out_size,
        "patch count");
    const std::size_t patch_size = checked_mul_size(
        checked_mul_size(channels_in_size, kernel_h_size, "patch size"),
        kernel_w_size,
        "patch size");
    const std::size_t out_channel_count = channels_out_size;
    const int matmul_m = checked_int_from_size(patch_count, "matmul M");
    const int matmul_k = checked_int_from_size(patch_size, "matmul K");
    const int matmul_n = checked_int_from_size(out_channel_count, "matmul N");

    const std::size_t input_image_size =
        checked_mul_size(height_size, width_size, "input image size");
    const std::size_t input_channel_size = checked_mul_size(
        channels_in_size, input_image_size, "input channel size");
    checked_mul_size(batch_size, input_channel_size, "input size");
    checked_mul_size(channels_out_size, patch_size, "kernel size");

    const std::size_t col_size =
        checked_mul_size(patch_count, patch_size, "im2col size");
    const std::size_t kernel_matrix_size =
        checked_mul_size(patch_size, out_channel_count, "kernel matrix size");
    const std::size_t output_matrix_size =
        checked_mul_size(patch_count, out_channel_count, "output matrix size");
    const std::size_t output_image_size =
        checked_mul_size(height_out_size, width_out_size, "output image size");
    const std::size_t output_channel_size = checked_mul_size(
        channels_out_size, output_image_size, "output channel size");
    checked_mul_size(batch_size, output_channel_size, "output size");

    std::vector<float> col(col_size);
    std::vector<float> kernel_matrix(kernel_matrix_size);
    std::vector<float> output_matrix(output_matrix_size);

    im2col(input,
           col.data(),
           batch,
           channels_in,
           height,
           width,
           kernel_h,
           kernel_w);
    reshape_kernel_for_im2col(kernel,
                              kernel_matrix.data(),
                              channels_out,
                              channels_in,
                              kernel_h,
                              kernel_w);
    matmul_vectorized(col.data(),
                      kernel_matrix.data(),
                      output_matrix.data(),
                      matmul_m,
                      matmul_k,
                      matmul_n);

    for (int row = 0; row < matmul_m; ++row) {
        const std::size_t row_size = static_cast<std::size_t>(row);
        const std::size_t n = row_size / output_image_size;
        const std::size_t output_position = row_size % output_image_size;
        const std::size_t oh = output_position / width_out_size;
        const std::size_t ow = output_position % width_out_size;
        for (int co = 0; co < channels_out; ++co) {
            const std::size_t output_index =
                ((n * channels_out_size + static_cast<std::size_t>(co)) *
                     height_out_size +
                 oh) *
                    width_out_size +
                ow;
            const std::size_t output_matrix_index =
                row_size * channels_out_size + static_cast<std::size_t>(co);
            output[output_index] = output_matrix[output_matrix_index];
        }
    }
}

} // namespace kernels
