# SLR Parsing Table (ACTION / GOTO)

## Grammar and Production Numbers

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

## FIRST and FOLLOW

- `FIRST(E) = { id, ( }`
- `FIRST(T) = { id, ( }`
- `FIRST(F) = { id, ( }`

- `FOLLOW(E) = { +, -, ), $ }`
- `FOLLOW(T) = { +, -, *, /, ), $ }`
- `FOLLOW(F) = { +, -, *, /, ), $ }`

## ACTION / GOTO Table

Legend:

- `sX` = shift and go to state `X`
- `rY` = reduce by production `Y`
- `acc` = accept
- empty cell = error

| St | id | +   | -   | *   | /   | (  | )   | $   | E  | T  | F  |
|---:|:--:|:---:|:---:|:---:|:---:|:--:|:---:|:---:|:--:|:--:|:--:|
| 0  | s5 |     |     |     |     | s4 |     |     | 1  | 2  | 3  |
| 1  |    | s6  | s7  |     |     |    |     | acc |    |    |    |
| 2  |    | r3  | r3  | s8  | s9  |    | r3  | r3  |    |    |    |
| 3  |    | r6  | r6  | r6  | r6  |    | r6  | r6  |    |    |    |
| 4  | s5 |     |     |     |     | s4 |     |     | 10 | 2  | 3  |
| 5  |    | r8  | r8  | r8  | r8  |    | r8  | r8  |    |    |    |
| 6  | s5 |     |     |     |     | s4 |     |     |    | 11 | 3  |
| 7  | s5 |     |     |     |     | s4 |     |     |    | 12 | 3  |
| 8  | s5 |     |     |     |     | s4 |     |     |    |    | 13 |
| 9  | s5 |     |     |     |     | s4 |     |     |    |    | 14 |
| 10 |    | s6  | s7  |     |     |    | s15 |     |    |    |    |
| 11 |    | r1  | r1  | s8  | s9  |    | r1  | r1  |    |    |    |
| 12 |    | r2  | r2  | s8  | s9  |    | r2  | r2  |    |    |    |
| 13 |    | r4  | r4  | r4  | r4  |    | r4  | r4  |    |    |    |
| 14 |    | r5  | r5  | r5  | r5  |    | r5  | r5  |    |    |    |
| 15 |    | r7  | r7  | r7  | r7  |    | r7  | r7  |    |    |    |

## How Reductions Were Placed

- State `2` has completed item `E -> T .`, so `r3` is added on `FOLLOW(E)`.
- State `3` has completed item `T -> F .`, so `r6` is added on `FOLLOW(T)`.
- State `5` has completed item `F -> id .`, so `r8` is added on `FOLLOW(F)`.
- State `11` has completed item `E -> E + T .`, so `r1` is added on `FOLLOW(E)`.
- State `12` has completed item `E -> E - T .`, so `r2` is added on `FOLLOW(E)`.
- State `13` has completed item `T -> T * F .`, so `r4` is added on `FOLLOW(T)`.
- State `14` has completed item `T -> T / F .`, so `r5` is added on `FOLLOW(T)`.
- State `15` has completed item `F -> ( E ) .`, so `r7` is added on `FOLLOW(F)`.
