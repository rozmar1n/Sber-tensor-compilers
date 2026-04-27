#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>

namespace kernels {

inline void require_positive(int value, const char* name)
{
    if (value <= 0) {
        throw std::invalid_argument(name);
    }
}

inline void require_non_null(const float* ptr, const char* name)
{
    if (ptr == nullptr) {
        throw std::invalid_argument(name);
    }
}

inline void require_non_null(float* ptr, const char* name)
{
    if (ptr == nullptr) {
        throw std::invalid_argument(name);
    }
}

inline std::size_t to_size(int value, const char* name)
{
    require_positive(value, name);
    return static_cast<std::size_t>(value);
}

inline std::size_t checked_add_size(std::size_t lhs,
                                    std::size_t rhs,
                                    const char* name)
{
    if (lhs > std::numeric_limits<std::size_t>::max() - rhs) {
        throw std::overflow_error(name);
    }
    return lhs + rhs;
}

inline std::size_t checked_mul_size(std::size_t lhs,
                                    std::size_t rhs,
                                    const char* name)
{
    if (rhs != 0 && lhs > std::numeric_limits<std::size_t>::max() / rhs) {
        throw std::overflow_error(name);
    }
    return lhs * rhs;
}

inline int checked_int_from_size(std::size_t value, const char* name)
{
    if (value > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::invalid_argument(name);
    }
    return static_cast<int>(value);
}

struct MatmulShape
{
    std::size_t m;
    std::size_t k;
    std::size_t n;
    std::size_t a_count;
    std::size_t b_count;
    std::size_t c_count;
};

inline MatmulShape checked_matmul_shape(int M, int K, int N)
{
    const std::size_t m = to_size(M, "M");
    const std::size_t k = to_size(K, "K");
    const std::size_t n = to_size(N, "N");

    return {
        m,
        k,
        n,
        checked_mul_size(m, k, "A element count"),
        checked_mul_size(k, n, "B element count"),
        checked_mul_size(m, n, "C element count"),
    };
}

} // namespace kernels
