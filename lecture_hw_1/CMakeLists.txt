cmake_minimum_required(VERSION 3.16)
project(lecture_hw_1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(FLEX REQUIRED)

FLEX_TARGET(
    LexerScanner
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.cpp
)

add_executable(slr_parser
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/slr_table.cpp
    ${FLEX_LexerScanner_OUTPUTS}
)

target_include_directories(slr_parser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(slr_parser PRIVATE ${FLEX_LIBRARIES})
