cmake_minimum_required(VERSION 3.16)

project(kernels LANGUAGES CXX)

option(KERNELS_ENABLE_AVX2 "Enable AVX2 kernels" OFF)
option(KERNELS_BUILD_TESTS "Build GoogleTest correctness tests" ON)
option(KERNELS_BUILD_BENCHMARKS "Build Google Benchmark executable" ON)

function(kernels_enable_warnings target_name)
    if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

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

target_compile_features(kernels PUBLIC cxx_std_17)
kernels_enable_warnings(kernels)

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" KERNELS_SYSTEM_PROCESSOR_LOWER)
set(KERNELS_AVX2_FLAGS_AVAILABLE OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$"
        AND KERNELS_SYSTEM_PROCESSOR_LOWER MATCHES "^(x86|x86_64|amd64|i386|i686)$")
    set(KERNELS_AVX2_FLAGS_AVAILABLE ON)
endif()

if(KERNELS_ENABLE_AVX2 AND KERNELS_AVX2_FLAGS_AVAILABLE)
    target_compile_definitions(kernels PRIVATE KERNELS_ENABLE_AVX2=1)
    set_source_files_properties(src/matmul_vectorized.cpp
        PROPERTIES
            COMPILE_OPTIONS -mavx2
    )
else()
    target_compile_definitions(kernels PRIVATE KERNELS_ENABLE_AVX2=0)
    if(KERNELS_ENABLE_AVX2)
        message(WARNING
            "KERNELS_ENABLE_AVX2 was requested, but AVX2 flags are only enabled "
            "for GNU/Clang on x86-family processors. Falling back to scalar/cache-friendly kernels."
        )
    endif()
endif()

if(KERNELS_BUILD_TESTS)
    find_package(GTest REQUIRED)

    add_executable(kernels_tests
        tests/test_main.cpp
        tests/test_matmul.cpp
        tests/test_conv2d.cpp
    )

    target_link_libraries(kernels_tests
        PRIVATE
            kernels
            GTest::gtest
    )
    kernels_enable_warnings(kernels_tests)

    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(kernels_tests)
endif()

if(KERNELS_BUILD_BENCHMARKS)
    find_package(benchmark REQUIRED)
    find_package(Threads REQUIRED)

    add_executable(kernels_bench
        benchmarks/kernels_bench.cpp
    )

    target_link_libraries(kernels_bench
        PRIVATE
            kernels
            benchmark::benchmark
            Threads::Threads
    )
    kernels_enable_warnings(kernels_bench)
endif()
