cmake_minimum_required(VERSION 3.16)

project(kernels LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(KERNELS_ENABLE_AVX2 "Enable AVX2 kernels" ON)

add_library(kernels STATIC
    src/matmul_naive.cpp
)

target_include_directories(kernels
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(kernels PRIVATE -Wall -Wextra -Wpedantic)

if(KERNELS_ENABLE_AVX2)
    target_compile_definitions(kernels PUBLIC KERNELS_ENABLE_AVX2=1)
    target_compile_options(kernels PRIVATE -mavx2 -mfma)
else()
    target_compile_definitions(kernels PUBLIC KERNELS_ENABLE_AVX2=0)
endif()

add_executable(kernels_tests
    tests/test_main.cpp
    tests/test_matmul.cpp
)

target_link_libraries(kernels_tests PRIVATE kernels)
target_compile_options(kernels_tests PRIVATE -Wall -Wextra -Wpedantic)

add_executable(kernels_bench
    benchmarks/kernels_bench.cpp
)

target_link_libraries(kernels_bench PRIVATE kernels)
target_compile_options(kernels_bench PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_test(NAME kernels_tests COMMAND kernels_tests)
