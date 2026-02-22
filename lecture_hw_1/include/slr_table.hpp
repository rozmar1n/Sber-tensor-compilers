#pragma once

#include <array>
#include <string>

#include "token.hpp"

enum class ActionType {
    SHIFT,
    REDUCE,
    ACCEPT,
    ERROR
};

struct Action {
    ActionType type;
    int value;
};

enum class NonTerminal {
    E = 0,
    T = 1,
    F = 2
};

struct ProductionInfo {
    NonTerminal lhs;
    const char* lhsName;
    const char* rhs;
    int rhsLength;
};

constexpr int kNumStates        = 16;
constexpr int kNumTerminals     = 8;    // id, +, -, *, /, (, ), $
constexpr int kNumNonTerminals  = 3;    // E, T, F
constexpr int kNumProductions   = 9;    // 0 is augmented grammar production

constexpr int kNonTerminalE = static_cast<int>(NonTerminal::E);
constexpr int kNonTerminalT = static_cast<int>(NonTerminal::T);
constexpr int kNonTerminalF = static_cast<int>(NonTerminal::F);

extern const std::array<std::array<Action, kNumTerminals>, kNumStates> ACTION_TABLE;
extern const std::array<std::array<int, kNumNonTerminals>, kNumStates> GOTO_TABLE;
extern const std::array<ProductionInfo, kNumProductions> PRODUCTIONS;

int terminalIndex(TokenType tokenType);
int nonTerminalIndex(NonTerminal nonTerminal);
Action actionFor(int state, TokenType lookahead);
int gotoState(int state, int nonTerminalIndex);
std::string actionToString(Action action);
