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

}  // namespace kernels
