#include "slr_table.hpp"

#include <stdexcept>

namespace {

Action errorAction() {
    return Action{ActionType::ERROR, -1};
}

}  // namespace

const std::array<ProductionInfo, kNumProductions> PRODUCTIONS = {{
    {NonTerminal::E, "E'", "E", 1},
    {NonTerminal::E, "E", "E + T", 3},
    {NonTerminal::E, "E", "E - T", 3},
    {NonTerminal::E, "E", "T", 1},
    {NonTerminal::T, "T", "T * F", 3},
    {NonTerminal::T, "T", "T / F", 3},
    {NonTerminal::T, "T", "F", 1},
    {NonTerminal::F, "F", "( E )", 3},
    {NonTerminal::F, "F", "id", 1},
}};

const std::array<std::array<Action, kNumTerminals>, kNumStates> ACTION_TABLE = [] {
    std::array<std::array<Action, kNumTerminals>, kNumStates> table{};
    for (auto& row : table) {
        row.fill(errorAction());
    }

    // State 0
    table[0][0] = Action{ActionType::SHIFT, 5};  // id
    table[0][5] = Action{ActionType::SHIFT, 1};  // (

    // State 1
    table[1][0] = Action{ActionType::SHIFT, 5};  // id
    table[1][5] = Action{ActionType::SHIFT, 1};  // (

    // State 2
    table[2][1] = Action{ActionType::SHIFT, 7};   // +
    table[2][2] = Action{ActionType::SHIFT, 8};   // -
    table[2][7] = Action{ActionType::ACCEPT, 0};  // $

    // State 3
    table[3][1] = Action{ActionType::REDUCE, 6};  // T -> F
    table[3][2] = Action{ActionType::REDUCE, 6};  // T -> F
    table[3][3] = Action{ActionType::REDUCE, 6};  // T -> F
    table[3][4] = Action{ActionType::REDUCE, 6};  // T -> F
    table[3][6] = Action{ActionType::REDUCE, 6};  // T -> F
    table[3][7] = Action{ActionType::REDUCE, 6};  // T -> F

    // State 4
    table[4][1] = Action{ActionType::REDUCE, 3};  // E -> T
    table[4][2] = Action{ActionType::REDUCE, 3};  // E -> T
    table[4][3] = Action{ActionType::SHIFT, 9};   // *
    table[4][4] = Action{ActionType::SHIFT, 10};  // /
    table[4][6] = Action{ActionType::REDUCE, 3};  // E -> T
    table[4][7] = Action{ActionType::REDUCE, 3};  // E -> T

    // State 5
    table[5][1] = Action{ActionType::REDUCE, 8};  // F -> id
    table[5][2] = Action{ActionType::REDUCE, 8};  // F -> id
    table[5][3] = Action{ActionType::REDUCE, 8};  // F -> id
    table[5][4] = Action{ActionType::REDUCE, 8};  // F -> id
    table[5][6] = Action{ActionType::REDUCE, 8};  // F -> id
    table[5][7] = Action{ActionType::REDUCE, 8};  // F -> id

    // State 6
    table[6][1] = Action{ActionType::SHIFT, 7};   // +
    table[6][2] = Action{ActionType::SHIFT, 8};   // -
    table[6][6] = Action{ActionType::SHIFT, 11};  // )

    // State 7
    table[7][0] = Action{ActionType::SHIFT, 5};  // id
    table[7][5] = Action{ActionType::SHIFT, 1};  // (

    // State 8
    table[8][0] = Action{ActionType::SHIFT, 5};  // id
    table[8][5] = Action{ActionType::SHIFT, 1};  // (

    // State 9
    table[9][0] = Action{ActionType::SHIFT, 5};  // id
    table[9][5] = Action{ActionType::SHIFT, 1};  // (

    // State 10
    table[10][0] = Action{ActionType::SHIFT, 5};  // id
    table[10][5] = Action{ActionType::SHIFT, 1};  // (

    // State 11
    table[11][1] = Action{ActionType::REDUCE, 7};  // F -> ( E )
    table[11][2] = Action{ActionType::REDUCE, 7};  // F -> ( E )
    table[11][3] = Action{ActionType::REDUCE, 7};  // F -> ( E )
    table[11][4] = Action{ActionType::REDUCE, 7};  // F -> ( E )
    table[11][6] = Action{ActionType::REDUCE, 7};  // F -> ( E )
    table[11][7] = Action{ActionType::REDUCE, 7};  // F -> ( E )

    // State 12
    table[12][1] = Action{ActionType::REDUCE, 1};  // E -> E + T
    table[12][2] = Action{ActionType::REDUCE, 1};  // E -> E + T
    table[12][3] = Action{ActionType::SHIFT, 9};   // *
    table[12][4] = Action{ActionType::SHIFT, 10};  // /
    table[12][6] = Action{ActionType::REDUCE, 1};  // E -> E + T
    table[12][7] = Action{ActionType::REDUCE, 1};  // E -> E + T

    // State 13
    table[13][1] = Action{ActionType::REDUCE, 2};  // E -> E - T
    table[13][2] = Action{ActionType::REDUCE, 2};  // E -> E - T
    table[13][3] = Action{ActionType::SHIFT, 9};   // *
    table[13][4] = Action{ActionType::SHIFT, 10};  // /
    table[13][6] = Action{ActionType::REDUCE, 2};  // E -> E - T
    table[13][7] = Action{ActionType::REDUCE, 2};  // E -> E - T

    // State 14
    table[14][1] = Action{ActionType::REDUCE, 4};  // T -> T * F
    table[14][2] = Action{ActionType::REDUCE, 4};  // T -> T * F
    table[14][3] = Action{ActionType::REDUCE, 4};  // T -> T * F
    table[14][4] = Action{ActionType::REDUCE, 4};  // T -> T * F
    table[14][6] = Action{ActionType::REDUCE, 4};  // T -> T * F
    table[14][7] = Action{ActionType::REDUCE, 4};  // T -> T * F

    // State 15
    table[15][1] = Action{ActionType::REDUCE, 5};  // T -> T / F
    table[15][2] = Action{ActionType::REDUCE, 5};  // T -> T / F
    table[15][3] = Action{ActionType::REDUCE, 5};  // T -> T / F
    table[15][4] = Action{ActionType::REDUCE, 5};  // T -> T / F
    table[15][6] = Action{ActionType::REDUCE, 5};  // T -> T / F
    table[15][7] = Action{ActionType::REDUCE, 5};  // T -> T / F

    return table;
}();

const std::array<std::array<int, kNumNonTerminals>, kNumStates> GOTO_TABLE = [] {
    std::array<std::array<int, kNumNonTerminals>, kNumStates> table{};
    for (auto& row : table) {
        row.fill(-1);
    }

    table[0][kNonTerminalE] = 2;
    table[0][kNonTerminalT] = 4;
    table[0][kNonTerminalF] = 3;

    table[1][kNonTerminalE] = 6;
    table[1][kNonTerminalT] = 4;
    table[1][kNonTerminalF] = 3;

    table[7][kNonTerminalT] = 12;
    table[7][kNonTerminalF] = 3;

    table[8][kNonTerminalT] = 13;
    table[8][kNonTerminalF] = 3;

    table[9][kNonTerminalF] = 14;
    table[10][kNonTerminalF] = 15;

    return table;
}();

int terminalIndex(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::ID:
            return 0;
        case TokenType::PLUS:
            return 1;
        case TokenType::MINUS:
            return 2;
        case TokenType::STAR:
            return 3;
        case TokenType::SLASH:
            return 4;
        case TokenType::LPAREN:
            return 5;
        case TokenType::RPAREN:
            return 6;
        case TokenType::END:
            return 7;
        case TokenType::INVALID:
            return -1;
    }
    return -1;
}

int nonTerminalIndex(NonTerminal nonTerminal) {
    return static_cast<int>(nonTerminal);
}

Action actionFor(int state, TokenType lookahead) {
    if (state < 0 || state >= kNumStates) {
        return errorAction();
    }

    const int termIndex = terminalIndex(lookahead);
    if (termIndex < 0 || termIndex >= kNumTerminals) {
        return errorAction();
    }
    return ACTION_TABLE[state][termIndex];
}

int gotoState(int state, int nonTerminalIndexValue) {
    if (state < 0 || state >= kNumStates) {
        return -1;
    }
    if (nonTerminalIndexValue < 0 || nonTerminalIndexValue >= kNumNonTerminals) {
        return -1;
    }
    return GOTO_TABLE[state][nonTerminalIndexValue];
}

std::string actionToString(Action action) {
    switch (action.type) {
        case ActionType::SHIFT:
            return "Shift s" + std::to_string(action.value);
        case ActionType::REDUCE:
            if (action.value >= 0 && action.value < kNumProductions) {
                const auto& production = PRODUCTIONS[action.value];
                return "Reduce r" + std::to_string(action.value) + " (" +
                       production.lhsName + " -> " + production.rhs + ")";
            }
            return "Reduce";
        case ActionType::ACCEPT:
            return "Accept";
        case ActionType::ERROR:
            return "Error";
    }
    throw std::runtime_error("Unknown action type.");
}
