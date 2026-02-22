# LR(0) Automaton

## Grammar

Terminals: `id`, `+`, `-`, `*`, `/`, `(`, `)`, `$`  
Non-terminals: `E`, `T`, `F`  
Start symbol: `E`

Augmented grammar:

- `0) E' -> E`
- `1) E -> E + T`
- `2) E -> E - T`
- `3) E -> T`
- `4) T -> T * F`
- `5) T -> T / F`
- `6) T -> F`
- `7) F -> ( E )`
- `8) F -> id`

## Canonical LR(0) Item Sets

### I0

- `E' -> . E`
- `E -> . E + T`
- `E -> . E - T`
- `E -> . T`
- `T -> . T * F`
- `T -> . T / F`
- `T -> . F`
- `F -> . ( E )`
- `F -> . id`

Transitions:

- `goto(I0, E) = I1`
- `goto(I0, T) = I2`
- `goto(I0, F) = I3`
- `goto(I0, '(') = I4`
- `goto(I0, id) = I5`

### I1

- `E' -> E .`
- `E -> E . + T`
- `E -> E . - T`

Transitions:

- `goto(I1, +) = I6`
- `goto(I1, -) = I7`

### I2

- `E -> T .`
- `T -> T . * F`
- `T -> T . / F`

Transitions:

- `goto(I2, *) = I8`
- `goto(I2, /) = I9`

### I3

- `T -> F .`

### I4

- `F -> ( . E )`
- `E -> . E + T`
- `E -> . E - T`
- `E -> . T`
- `T -> . T * F`
- `T -> . T / F`
- `T -> . F`
- `F -> . ( E )`
- `F -> . id`

Transitions:

- `goto(I4, E) = I10`
- `goto(I4, T) = I2`
- `goto(I4, F) = I3`
- `goto(I4, '(') = I4`
- `goto(I4, id) = I5`

### I5

- `F -> id .`

### I6

- `E -> E + . T`
- `T -> . T * F`
- `T -> . T / F`
- `T -> . F`
- `F -> . ( E )`
- `F -> . id`

Transitions:

- `goto(I6, T) = I11`
- `goto(I6, F) = I3`
- `goto(I6, '(') = I4`
- `goto(I6, id) = I5`

### I7

- `E -> E - . T`
- `T -> . T * F`
- `T -> . T / F`
- `T -> . F`
- `F -> . ( E )`
- `F -> . id`

Transitions:

- `goto(I7, T) = I12`
- `goto(I7, F) = I3`
- `goto(I7, '(') = I4`
- `goto(I7, id) = I5`

### I8

- `T -> T * . F`
- `F -> . ( E )`
- `F -> . id`

Transitions:

- `goto(I8, F) = I13`
- `goto(I8, '(') = I4`
- `goto(I8, id) = I5`

### I9

- `T -> T / . F`
- `F -> . ( E )`
- `F -> . id`

Transitions:

- `goto(I9, F) = I14`
- `goto(I9, '(') = I4`
- `goto(I9, id) = I5`

### I10

- `F -> ( E . )`
- `E -> E . + T`
- `E -> E . - T`

Transitions:

- `goto(I10, ')') = I15`
- `goto(I10, +) = I6`
- `goto(I10, -) = I7`

### I11

- `E -> E + T .`
- `T -> T . * F`
- `T -> T . / F`

Transitions:

- `goto(I11, *) = I8`
- `goto(I11, /) = I9`

### I12

- `E -> E - T .`
- `T -> T . * F`
- `T -> T . / F`

Transitions:

- `goto(I12, *) = I8`
- `goto(I12, /) = I9`

### I13

- `T -> T * F .`

### I14

- `T -> T / F .`

### I15

- `F -> ( E ) .`

## Transition Summary

| From | Symbol | To  |
|-----:|:------:|----:|
| I0   | `E`    | I1  |
| I0   | `T`    | I2  |
| I0   | `F`    | I3  |
| I0   | `(`    | I4  |
| I0   | `id`   | I5  |
| I1   | `+`    | I6  |
| I1   | `-`    | I7  |
| I2   | `*`    | I8  |
| I2   | `/`    | I9  |
| I4   | `E`    | I10 |
| I4   | `T`    | I2  |
| I4   | `F`    | I3  |
| I4   | `(`    | I4  |
| I4   | `id`   | I5  |
| I6   | `T`    | I11 |
| I6   | `F`    | I3  |
| I6   | `(`    | I4  |
| I6   | `id`   | I5  |
| I7   | `T`    | I12 |
| I7   | `F`    | I3  |
| I7   | `(`    | I4  |
| I7   | `id`   | I5  |
| I8   | `F`    | I13 |
| I8   | `(`    | I4  |
| I8   | `id`   | I5  |
| I9   | `F`    | I14 |
| I9   | `(`    | I4  |
| I9   | `id`   | I5  |
| I10  | `)`    | I15 |
| I10  | `+`    | I6  |
| I10  | `-`    | I7  |
| I11  | `*`    | I8  |
| I11  | `/`    | I9  |
| I12  | `*`    | I8  |
| I12  | `/`    | I9  |
