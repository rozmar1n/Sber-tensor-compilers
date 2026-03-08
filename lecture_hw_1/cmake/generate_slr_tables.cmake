cmake_minimum_required(VERSION 3.16)

foreach(required_var ACTION_CSV GOTO_CSV OUTPUT_HEADER)
    if (NOT DEFINED ${required_var})
        message(FATAL_ERROR "Missing required variable: ${required_var}")
    endif()
endforeach()

set(EXPECTED_ACTION_ROWS 16)
set(EXPECTED_ACTION_COLS 8)
set(EXPECTED_GOTO_ROWS 16)
set(EXPECTED_GOTO_COLS 3)
set(MAX_STATE_INDEX 15)

function(read_csv_strict path expected_rows expected_cols out_rows)
    file(READ "${path}" raw_content)
    string(REPLACE "\r\n" "\n" raw_content "${raw_content}")
    string(REPLACE "\r" "\n" raw_content "${raw_content}")

    while (raw_content MATCHES "\n$")
        string(REGEX REPLACE "\n$" "" raw_content "${raw_content}")
    endwhile()

    if (raw_content STREQUAL "")
        message(FATAL_ERROR "CSV file is empty: ${path}")
    endif()

    string(REPLACE "\n" ";" lines "${raw_content}")
    list(LENGTH lines line_count)
    if (NOT line_count EQUAL expected_rows)
        message(FATAL_ERROR
            "CSV row count mismatch for ${path}: expected ${expected_rows}, got ${line_count}")
    endif()

    set(parsed_rows "")
    set(row_index 0)
    foreach(line IN LISTS lines)
        math(EXPR row_index "${row_index} + 1")
        if (line STREQUAL "")
            message(FATAL_ERROR "Blank line is not allowed: ${path}:${row_index}")
        endif()
        if (line MATCHES "#")
            message(FATAL_ERROR "Comments are not allowed: ${path}:${row_index}")
        endif()

        string(REPLACE "," ";" cells "${line}")
        list(LENGTH cells cell_count)
        if (NOT cell_count EQUAL expected_cols)
            message(FATAL_ERROR
                "CSV column count mismatch in ${path}:${row_index}: expected ${expected_cols}, got ${cell_count}")
        endif()

        set(normalized_cells "")
        set(col_index 0)
        foreach(cell IN LISTS cells)
            math(EXPR col_index "${col_index} + 1")
            string(STRIP "${cell}" stripped)
            if (stripped STREQUAL "")
                message(FATAL_ERROR "Empty cell is not allowed: ${path}:${row_index}:${col_index}")
            endif()
            if (NOT "${cell}" STREQUAL "${stripped}")
                message(FATAL_ERROR
                    "Leading/trailing spaces are not allowed: ${path}:${row_index}:${col_index}")
            endif()
            list(APPEND normalized_cells "${stripped}")
        endforeach()

        string(JOIN "," normalized_row ${normalized_cells})
        list(APPEND parsed_rows "${normalized_row}")
    endforeach()

    set(${out_rows} "${parsed_rows}" PARENT_SCOPE)
endfunction()

function(action_literal token out_literal)
    if (token STREQUAL "err")
        set(literal "Action{ActionType::ERROR, -1}")
    elseif (token STREQUAL "acc")
        set(literal "Action{ActionType::ACCEPT, 0}")
    elseif (token MATCHES "^s([0-9]+)$")
        set(literal "Action{ActionType::SHIFT, ${CMAKE_MATCH_1}}")
    elseif (token MATCHES "^r([0-9]+)$")
        set(literal "Action{ActionType::REDUCE, ${CMAKE_MATCH_1}}")
    else()
        message(FATAL_ERROR
            "Invalid ACTION token '${token}'. Allowed: sN, rN, acc, err")
    endif()
    set(${out_literal} "${literal}" PARENT_SCOPE)
endfunction()

function(goto_literal token out_literal)
    if (NOT token MATCHES "^-?[0-9]+$")
        message(FATAL_ERROR "Invalid GOTO token '${token}'. Expected integer.")
    endif()

    if (token LESS -1 OR token GREATER ${MAX_STATE_INDEX})
        message(FATAL_ERROR
            "GOTO token '${token}' out of allowed range [-1, ${MAX_STATE_INDEX}]")
    endif()

    set(${out_literal} "${token}" PARENT_SCOPE)
endfunction()

read_csv_strict("${ACTION_CSV}" ${EXPECTED_ACTION_ROWS} ${EXPECTED_ACTION_COLS} ACTION_ROWS)
read_csv_strict("${GOTO_CSV}" ${EXPECTED_GOTO_ROWS} ${EXPECTED_GOTO_COLS} GOTO_ROWS)

file(WRITE "${OUTPUT_HEADER}" "#pragma once\n\n")
file(APPEND "${OUTPUT_HEADER}" "#include <array>\n\n")
file(APPEND "${OUTPUT_HEADER}" "#include \"slr_table.hpp\"\n\n")
file(APPEND "${OUTPUT_HEADER}"
    "constexpr std::array<std::array<Action, kNumTerminals>, kNumStates> GENERATED_ACTION_TABLE = {{\n")

foreach(row IN LISTS ACTION_ROWS)
    string(REPLACE "," ";" cells "${row}")
    set(action_literals "")
    foreach(cell IN LISTS cells)
        action_literal("${cell}" literal)
        list(APPEND action_literals "${literal}")
    endforeach()
    string(JOIN ", " action_row ${action_literals})
    file(APPEND "${OUTPUT_HEADER}" "    std::array<Action, kNumTerminals>{{${action_row}}},\n")
endforeach()

file(APPEND "${OUTPUT_HEADER}" "}};\n\n")
file(APPEND "${OUTPUT_HEADER}"
    "constexpr std::array<std::array<int, kNumNonTerminals>, kNumStates> GENERATED_GOTO_TABLE = {{\n")

foreach(row IN LISTS GOTO_ROWS)
    string(REPLACE "," ";" cells "${row}")
    set(goto_literals "")
    foreach(cell IN LISTS cells)
        goto_literal("${cell}" literal)
        list(APPEND goto_literals "${literal}")
    endforeach()
    string(JOIN ", " goto_row ${goto_literals})
    file(APPEND "${OUTPUT_HEADER}" "    std::array<int, kNumNonTerminals>{{${goto_row}}},\n")
endforeach()

file(APPEND "${OUTPUT_HEADER}" "}};\n")
