#include "kernels/conv2d.hpp"

#include "kernels/utils.hpp"

namespace kernels {

void im2col(const float* input,
            float* col,
            int batch,
            int channels_in,
            int height,
            int width,
            int kernel_h,
            int kernel_w)
{
    require_non_null(input, "input");
    require_non_null(col, "col");
    require_positive(batch, "batch");
    require_positive(channels_in, "channels_in");
    require_positive(height, "height");
    require_positive(width, "width");
    require_positive(kernel_h, "kernel_h");
    require_positive(kernel_w, "kernel_w");

    const int height_out = conv2d_output_size(height, kernel_h);
    const int width_out = conv2d_output_size(width, kernel_w);
    const std::size_t batch_size = to_size(batch, "batch");
    const std::size_t channels_in_size = to_size(channels_in, "channels_in");
    const std::size_t height_size = to_size(height, "height");
    const std::size_t width_size = to_size(width, "width");
    const std::size_t kernel_h_size = to_size(kernel_h, "kernel_h");
    const std::size_t kernel_w_size = to_size(kernel_w, "kernel_w");
    const std::size_t height_out_size = to_size(height_out, "height_out");
    const std::size_t width_out_size = to_size(width_out, "width_out");

    const std::size_t patch_size = checked_mul_size(
        checked_mul_size(channels_in_size, kernel_h_size, "patch size"),
        kernel_w_size,
        "patch size");
    const std::size_t col_rows = checked_mul_size(
        checked_mul_size(batch_size, height_out_size, "im2col rows"),
        width_out_size,
        "im2col rows");
    checked_mul_size(col_rows, patch_size, "im2col size");

    const std::size_t input_image_size =
        checked_mul_size(height_size, width_size, "input image size");
    const std::size_t input_channel_size = checked_mul_size(
        channels_in_size, input_image_size, "input channel size");
    checked_mul_size(batch_size, input_channel_size, "input size");

    for (int n = 0; n < batch; ++n) {
        for (int oh = 0; oh < height_out; ++oh) {
            for (int ow = 0; ow < width_out; ++ow) {
                const std::size_t row =
                    (static_cast<std::size_t>(n) * height_out_size +
                     static_cast<std::size_t>(oh)) *
                        width_out_size +
                    static_cast<std::size_t>(ow);
                for (int ci = 0; ci < channels_in; ++ci) {
                    for (int kh = 0; kh < kernel_h; ++kh) {
                        for (int kw = 0; kw < kernel_w; ++kw) {
                            const std::size_t column =
                                (static_cast<std::size_t>(ci) * kernel_h_size +
                                 static_cast<std::size_t>(kh)) *
                                    kernel_w_size +
                                static_cast<std::size_t>(kw);
                            const std::size_t input_index =
                                ((static_cast<std::size_t>(n) *
                                      channels_in_size +
                                  static_cast<std::size_t>(ci)) *
                                     height_size +
                                 static_cast<std::size_t>(oh + kh)) *
                                    width_size +
                                static_cast<std::size_t>(ow + kw);
                            col[row * patch_size + column] = input[input_index];
                        }
                    }
                }
            }
        }
    }
}

void reshape_kernel_for_im2col(const float* kernel,
                               float* kernel_matrix,
                               int channels_out,
                               int channels_in,
                               int kernel_h,
                               int kernel_w)
{
    require_non_null(kernel, "kernel");
    require_non_null(kernel_matrix, "kernel_matrix");
    require_positive(channels_out, "channels_out");
    require_positive(channels_in, "channels_in");
    require_positive(kernel_h, "kernel_h");
    require_positive(kernel_w, "kernel_w");

    const std::size_t channels_out_size = to_size(channels_out, "channels_out");
    const std::size_t channels_in_size = to_size(channels_in, "channels_in");
    const std::size_t kernel_h_size = to_size(kernel_h, "kernel_h");
    const std::size_t kernel_w_size = to_size(kernel_w, "kernel_w");
    const std::size_t kernel_spatial_size =
        checked_mul_size(kernel_h_size, kernel_w_size, "kernel spatial size");
    const std::size_t kernel_input_size = checked_mul_size(
        channels_in_size, kernel_spatial_size, "kernel input size");
    checked_mul_size(channels_out_size, kernel_input_size, "kernel size");
    checked_mul_size(
        kernel_input_size, channels_out_size, "kernel matrix size");

    // OIHW kernel layout becomes [ci, kh, kw, co] row-major GEMM layout.
    for (int co = 0; co < channels_out; ++co) {
        for (int ci = 0; ci < channels_in; ++ci) {
            for (int kh = 0; kh < kernel_h; ++kh) {
                for (int kw = 0; kw < kernel_w; ++kw) {
                    const std::size_t matrix_row =
                        (static_cast<std::size_t>(ci) * kernel_h_size +
                         static_cast<std::size_t>(kh)) *
                            kernel_w_size +
                        static_cast<std::size_t>(kw);
                    const std::size_t matrix_index =
                        matrix_row * channels_out_size +
                        static_cast<std::size_t>(co);
                    const std::size_t kernel_index =
                        ((static_cast<std::size_t>(co) * channels_in_size +
                          static_cast<std::size_t>(ci)) *
                             kernel_h_size +
                         static_cast<std::size_t>(kh)) *
                            kernel_w_size +
                        static_cast<std::size_t>(kw);
                    kernel_matrix[matrix_index] = kernel[kernel_index];
                }
            }
        }
    }
}

} // namespace kernels
