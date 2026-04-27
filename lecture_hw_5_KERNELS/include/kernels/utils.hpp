#pragma once

#include <stdexcept>

namespace kernels {

inline void require_positive(int value, const char* name) {
    if (value <= 0) {
        throw std::invalid_argument(name);
    }
}

inline void require_non_null(const float* ptr, const char* name) {
    if (ptr == nullptr) {
        throw std::invalid_argument(name);
    }
}

inline void require_non_null(float* ptr, const char* name) {
    if (ptr == nullptr) {
        throw std::invalid_argument(name);
    }
}

}  // namespace kernels
