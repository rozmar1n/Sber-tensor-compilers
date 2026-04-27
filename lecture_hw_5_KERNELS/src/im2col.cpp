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
            int kernel_w) {
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
    const int col_width = channels_in * kernel_h * kernel_w;

    for (int n = 0; n < batch; ++n) {
        for (int oh = 0; oh < height_out; ++oh) {
            for (int ow = 0; ow < width_out; ++ow) {
                const int row = (n * height_out + oh) * width_out + ow;
                for (int ci = 0; ci < channels_in; ++ci) {
                    for (int kh = 0; kh < kernel_h; ++kh) {
                        for (int kw = 0; kw < kernel_w; ++kw) {
                            const int column = (ci * kernel_h + kh) * kernel_w + kw;
                            const int input_index =
                                ((n * channels_in + ci) * height + (oh + kh)) * width +
                                (ow + kw);
                            col[row * col_width + column] = input[input_index];
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
                               int kernel_w) {
    require_non_null(kernel, "kernel");
    require_non_null(kernel_matrix, "kernel_matrix");
    require_positive(channels_out, "channels_out");
    require_positive(channels_in, "channels_in");
    require_positive(kernel_h, "kernel_h");
    require_positive(kernel_w, "kernel_w");

    for (int co = 0; co < channels_out; ++co) {
        for (int ci = 0; ci < channels_in; ++ci) {
            for (int kh = 0; kh < kernel_h; ++kh) {
                for (int kw = 0; kw < kernel_w; ++kw) {
                    const int matrix_row = (ci * kernel_h + kh) * kernel_w + kw;
                    kernel_matrix[matrix_row * channels_out + co] =
                        kernel[((co * channels_in + ci) * kernel_h + kh) * kernel_w + kw];
                }
            }
        }
    }
}

}  // namespace kernels
