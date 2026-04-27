#pragma once

#include <stdexcept>

namespace kernels {

inline int conv2d_output_size(int input_size, int kernel_size)
{
    if (input_size <= 0) {
        throw std::invalid_argument("input_size");
    }
    if (kernel_size <= 0) {
        throw std::invalid_argument("kernel_size");
    }
    if (kernel_size > input_size) {
        throw std::invalid_argument("kernel_size");
    }
    return input_size - kernel_size + 1;
}

// Conv2D uses contiguous float tensors:
// input[N, C_in, H, W] at ((n * C_in + ci) * H + h) * W + w,
// kernel[C_out, C_in, K_h, K_w] at ((co * C_in + ci) * K_h + kh) * K_w + kw,
// output[N, C_out, H_out, W_out] with valid convolution only.
// Input and output buffers must be non-null, large enough, and non-overlapping.
void conv2d_naive(const float* input,
                  const float* kernel,
                  float* output,
                  int batch,
                  int channels_in,
                  int height,
                  int width,
                  int channels_out,
                  int kernel_h,
                  int kernel_w);

// Writes row-major col[batch * H_out * W_out, channels_in * kernel_h *
// kernel_w].
void im2col(const float* input,
            float* col,
            int batch,
            int channels_in,
            int height,
            int width,
            int kernel_h,
            int kernel_w);

// Converts kernel[C_out, C_in, K_h, K_w] into row-major
// kernel_matrix[C_in * K_h * K_w, C_out] for Im2Col + MatMul.
void reshape_kernel_for_im2col(const float* kernel,
                               float* kernel_matrix,
                               int channels_out,
                               int channels_in,
                               int kernel_h,
                               int kernel_w);

// Computes valid Conv2D through Im2Col, kernel reshape, MatMul, and output
// layout conversion back to NCHW.
void conv2d_im2col(const float* input,
                   const float* kernel,
                   float* output,
                   int batch,
                   int channels_in,
                   int height,
                   int width,
                   int channels_out,
                   int kernel_h,
                   int kernel_w);

} // namespace kernels
