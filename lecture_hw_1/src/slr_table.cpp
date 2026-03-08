#include "slr_table.hpp"

#include <stdexcept>

#include "slr_tables_generated.hpp"

namespace {

Action errorAction() {
    return Action{ActionType::ERROR, -1};
}

static_assert(static_cast<int>(TokenType::ID) == 0, "TokenType::ID must map to terminal 0.");
static_assert(static_cast<int>(TokenType::PLUS) == 1, "TokenType::PLUS must map to terminal 1.");
static_assert(static_cast<int>(TokenType::MINUS) == 2, "TokenType::MINUS must map to terminal 2.");
static_assert(static_cast<int>(TokenType::STAR) == 3, "TokenType::STAR must map to terminal 3.");
static_assert(static_cast<int>(TokenType::SLASH) == 4, "TokenType::SLASH must map to terminal 4.");
static_assert(static_cast<int>(TokenType::LPAREN) == 5,
              "TokenType::LPAREN must map to terminal 5.");
static_assert(static_cast<int>(TokenType::RPAREN) == 6,
              "TokenType::RPAREN must map to terminal 6.");
static_assert(static_cast<int>(TokenType::END) == kNumTerminals - 1,
              "TokenType::END must map to the last terminal column.");
static_assert(static_cast<int>(TokenType::INVALID) == kNumTerminals,
              "TokenType::INVALID must stay outside ACTION columns.");

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

const std::array<std::array<Action, kNumTerminals>, kNumStates> ACTION_TABLE =
    GENERATED_ACTION_TABLE;

const std::array<std::array<int, kNumNonTerminals>, kNumStates> GOTO_TABLE = GENERATED_GOTO_TABLE;

int terminalIndex(TokenType tokenType) {
    const int index = static_cast<int>(tokenType);
    if (index < 0 || index >= kNumTerminals) {
        return -1;
    }
    return index;
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
