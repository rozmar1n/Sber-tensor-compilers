#pragma once

#include <cstddef>
#include <limits>
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

inline std::size_t to_size(int value, const char* name) {
    require_positive(value, name);
    return static_cast<std::size_t>(value);
}

inline std::size_t checked_add_size(std::size_t lhs, std::size_t rhs, const char* name) {
    if (lhs > std::numeric_limits<std::size_t>::max() - rhs) {
        throw std::overflow_error(name);
    }
    return lhs + rhs;
}

inline std::size_t checked_mul_size(std::size_t lhs, std::size_t rhs, const char* name) {
    if (rhs != 0 && lhs > std::numeric_limits<std::size_t>::max() / rhs) {
        throw std::overflow_error(name);
    }
    return lhs * rhs;
}

inline int checked_int_from_size(std::size_t value, const char* name) {
    if (value > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::invalid_argument(name);
    }
    return static_cast<int>(value);
}

}  // namespace kernels
