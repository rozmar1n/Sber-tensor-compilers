cmake_minimum_required(VERSION 3.16)
project(lecture_hw_1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
enable_testing()

find_package(FLEX REQUIRED)

FLEX_TARGET(
    LexerScanner
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.cpp
)

add_library(slr_core
    src/lexer.cpp
    src/parser.cpp
    src/slr_table.cpp
    ${FLEX_LexerScanner_OUTPUTS}
)

target_include_directories(slr_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(slr_core PUBLIC ${FLEX_LIBRARIES})

add_executable(slr_parser
    src/main.cpp
)

target_link_libraries(slr_parser PRIVATE slr_core)

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(slr_parser_tests
    tests/parser_gtest.cpp
)

target_compile_definitions(slr_parser_tests PRIVATE
    HW1_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

if (TARGET GTest::gtest_main)
    target_link_libraries(slr_parser_tests PRIVATE slr_core GTest::gtest_main)
elseif (TARGET GTest::Main AND TARGET GTest::GTest)
    target_link_libraries(slr_parser_tests PRIVATE slr_core GTest::Main GTest::GTest)
else()
    message(FATAL_ERROR "Unsupported GTest package: no gtest_main target found.")
endif()

gtest_discover_tests(slr_parser_tests)
