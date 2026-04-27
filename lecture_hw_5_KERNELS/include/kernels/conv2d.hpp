#pragma once

#include <stdexcept>

namespace kernels {

inline int conv2d_output_size(int input_size, int kernel_size) {
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

void im2col(const float* input,
            float* col,
            int batch,
            int channels_in,
            int height,
            int width,
            int kernel_h,
            int kernel_w);

void reshape_kernel_for_im2col(const float* kernel,
                               float* kernel_matrix,
                               int channels_out,
                               int channels_in,
                               int kernel_h,
                               int kernel_w);

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

}  // namespace kernels
