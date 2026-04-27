cmake_minimum_required(VERSION 3.16)

project(kernels LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(KERNELS_ENABLE_AVX2 "Enable AVX2 kernels" OFF)

add_library(kernels STATIC
    src/matmul_naive.cpp
    src/matmul_cache_friendly.cpp
    src/matmul_tiled.cpp
    src/matmul_vectorized.cpp
    src/conv2d_naive.cpp
    src/im2col.cpp
    src/conv2d_im2col.cpp
)

target_include_directories(kernels
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(kernels PRIVATE -Wall -Wextra -Wpedantic)

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" KERNELS_SYSTEM_PROCESSOR_LOWER)
set(KERNELS_AVX2_SUPPORTED OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$"
        AND KERNELS_SYSTEM_PROCESSOR_LOWER MATCHES "^(x86|x86_64|amd64|i386|i686)$")
    set(KERNELS_AVX2_SUPPORTED ON)
endif()

if(KERNELS_ENABLE_AVX2 AND KERNELS_AVX2_SUPPORTED)
    target_compile_definitions(kernels PUBLIC KERNELS_ENABLE_AVX2=1)
    target_compile_options(kernels PRIVATE -mavx2 -mfma)
else()
    target_compile_definitions(kernels PUBLIC KERNELS_ENABLE_AVX2=0)
    if(KERNELS_ENABLE_AVX2)
        message(WARNING
            "KERNELS_ENABLE_AVX2 was requested, but AVX2 flags are only enabled "
            "for GNU/Clang on x86-family processors. Falling back to scalar/cache-friendly kernels."
        )
    endif()
endif()

add_executable(kernels_tests
    tests/test_main.cpp
    tests/test_matmul.cpp
    tests/test_conv2d.cpp
)

target_link_libraries(kernels_tests PRIVATE kernels)
target_compile_options(kernels_tests PRIVATE -Wall -Wextra -Wpedantic)

find_package(benchmark REQUIRED)

add_executable(kernels_bench
    benchmarks/kernels_bench.cpp
)

target_link_libraries(kernels_bench
    PRIVATE
        kernels
        benchmark::benchmark
)
target_compile_options(kernels_bench PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_test(NAME kernels_tests COMMAND kernels_tests)
