#include "kernels/conv2d.hpp"

#include "kernels/utils.hpp"

namespace kernels {

void conv2d_naive(const float* input,
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

    for (int n = 0; n < batch; ++n) {
        for (int co = 0; co < channels_out; ++co) {
            for (int oh = 0; oh < height_out; ++oh) {
                for (int ow = 0; ow < width_out; ++ow) {
                    float sum = 0.0f;
                    for (int ci = 0; ci < channels_in; ++ci) {
                        for (int kh = 0; kh < kernel_h; ++kh) {
                            for (int kw = 0; kw < kernel_w; ++kw) {
                                const int input_index =
                                    ((n * channels_in + ci) * height + (oh + kh)) * width +
                                    (ow + kw);
                                const int kernel_index =
                                    ((co * channels_in + ci) * kernel_h + kh) * kernel_w + kw;
                                sum += input[input_index] * kernel[kernel_index];
                            }
                        }
                    }
                    output[((n * channels_out + co) * height_out + oh) * width_out + ow] = sum;
                }
            }
        }
    }
}

}  // namespace kernels
