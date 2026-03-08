# SLR Parser (Lecture HW 1)

Implementation of an SLR(1) shift/reduce parser for an arithmetic language:

- binary operators: `+`, `-`, `*`, `/`
- parentheses: `(`, `)`
- operands: numbers and identifiers
- identifiers: Latin letters only `[a-zA-Z]+`
- lexer implemented with Flex
- parser implemented manually

## 1. Grammar

Terminals: `id`, `+`, `-`, `*`, `/`, `(`, `)`, `$`  
Non-terminals: `E`, `T`, `F`  
Start symbol: `E`

Augmented grammar:

0. `E' -> E`
1. `E -> E + T`
2. `E -> E - T`
3. `E -> T`
4. `T -> T * F`
5. `T -> T / F`
6. `T -> F`
7. `F -> ( E )`
8. `F -> id`

## 2. FIRST/FOLLOW

- `FIRST(E) = { id, ( }`
- `FIRST(T) = { id, ( }`
- `FIRST(F) = { id, ( }`

- `FOLLOW(E) = { +, -, ), $ }`
- `FOLLOW(T) = { +, -, *, /, ), $ }`
- `FOLLOW(F) = { +, -, *, /, ), $ }`

## 3. LR(0) and SLR(1)

Detailed derivation is in `docs`:

- LR(0) automaton: `docs/lr0_automaton.md`
- ACTION/GOTO table: `docs/slr_table.md`

In code, parser tables are stored as CSV and embedded at build time:

- `tables/action_table.csv`
- `tables/goto_table.csv`
- generated header: `build/generated/slr_tables_generated.hpp`

## 4. Project Structure

```text
lecture_hw_1/
├── include/
│   ├── token.hpp
│   ├── lexer.hpp
│   ├── parser.hpp
│   └── slr_table.hpp
├── src/
│   ├── main.cpp
│   ├── lexer.cpp
│   ├── parser.cpp
│   └── slr_table.cpp
├── lexer/
│   └── lexer.l
├── docs/
│   ├── lr0_automaton.md
│   └── slr_table.md
├── tables/
│   ├── action_table.csv
│   └── goto_table.csv
├── cmake/
│   └── generate_slr_tables.cmake
├── tests/
│   ├── test_valid.txt
│   ├── test_valid_precedence.txt
│   ├── test_invalid.txt
│   ├── test_invalid_parentheses.txt
│   └── test_invalid_identifier.txt
├── CMakeLists.txt
└── README.md
```

## 5. Build

Requirements:

- `cmake >= 3.16`
- C++17 compiler
- `flex`

```bash
cmake -S . -B build
cmake --build build
```

## 6. Run

From a file:

```bash
./build/slr_parser <file>
```

From stdin:

```bash
echo "(a + 2) * b - 10 / c" | ./build/slr_parser
```

## 7. Table Generation

During `cmake --build build`, CMake runs `cmake/generate_slr_tables.cmake` and generates:

- `build/generated/slr_tables_generated.hpp`

The generator uses strict CSV validation:

- no headers
- no comments
- no empty lines
- fixed table sizes (ACTION: `16x8`, GOTO: `16x3`)

## 8. Parsing Visualization

Each step prints:

- `STACK`: stack in `symbol state` format
- `INPUT`: remaining tokens
- `ACTION`: parser action (`Shift`, `Reduce`, `Accept`, `Error`)

Example:

```text
STACK                                  | INPUT                              | ACTION
$ 0                                    | ( alpha + 12 ) * beta $            | Shift s1
$ 0 ( 1                                | alpha + 12 ) * beta $              | Shift s5
$ 0 ( 1 alpha 5                        | + 12 ) * beta $                    | Reduce r8 (F -> id)
...
Result: ACCEPT
```

## 9. Assignment Constraints

- Flex is used only for tokenization.
- Bison and other parser generators are not used.
- The SLR ACTION/GOTO table is maintained manually in CSV and embedded at build time.
