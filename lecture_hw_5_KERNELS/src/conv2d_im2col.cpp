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
                   int kernel_w) {
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
    const int m = batch * height_out * width_out;
    const int k = channels_in * kernel_h * kernel_w;
    const int n_mat = channels_out;

    std::vector<float> col(static_cast<std::size_t>(m) * static_cast<std::size_t>(k));
    std::vector<float> kernel_matrix(static_cast<std::size_t>(k) *
                                     static_cast<std::size_t>(n_mat));
    std::vector<float> output_matrix(static_cast<std::size_t>(m) *
                                     static_cast<std::size_t>(n_mat));

    im2col(input, col.data(), batch, channels_in, height, width, kernel_h, kernel_w);
    reshape_kernel_for_im2col(kernel, kernel_matrix.data(), channels_out, channels_in,
                              kernel_h, kernel_w);
    matmul_vectorized(col.data(), kernel_matrix.data(), output_matrix.data(), m, k, n_mat);

    for (int row = 0; row < m; ++row) {
        const int n = row / (height_out * width_out);
        const int output_position = row % (height_out * width_out);
        const int oh = output_position / width_out;
        const int ow = output_position % width_out;
        for (int co = 0; co < channels_out; ++co) {
            output[((n * channels_out + co) * height_out + oh) * width_out + ow] =
                output_matrix[row * channels_out + co];
        }
    }
}

}  // namespace kernels
