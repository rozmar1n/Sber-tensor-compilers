#include "slr_table.hpp"

#include <stdexcept>

namespace {

Action errorAction() {
    return Action{ActionType::ERROR, -1};
}

Action shiftAction(int state) {
    return Action{ActionType::SHIFT, state};
}

Action reduceAction(int production) {
    return Action{ActionType::REDUCE, production};
}

Action acceptAction() {
    return Action{ActionType::ACCEPT, 0};
}

enum class Terminal : int {
    ID = 0,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    LPAREN,
    RPAREN,
    END
};

using ActionRow = std::array<Action, kNumTerminals>;
using ActionTable = std::array<ActionRow, kNumStates>;
using GotoRow = std::array<int, kNumNonTerminals>;
using GotoTable = std::array<GotoRow, kNumStates>;

constexpr int terminalToIndex(Terminal terminal) {
    return static_cast<int>(terminal);
}

void setAction(ActionTable& table, int state, Terminal terminal, Action action) {
    table[state][terminalToIndex(terminal)] = action;
}

void setShift(ActionTable& table, int state, Terminal terminal, int nextState) {
    setAction(table, state, terminal, shiftAction(nextState));
}

template <std::size_t N>
void setReduce(ActionTable& table, int state, const std::array<Terminal, N>& terminals,
               int production) {
    for (Terminal terminal : terminals) {
        setAction(table, state, terminal, reduceAction(production));
    }
}

void setAccept(ActionTable& table, int state, Terminal terminal) {
    setAction(table, state, terminal, acceptAction());
}

void setGoto(GotoTable& table, int state, NonTerminal nonTerminal, int nextState) {
    table[state][static_cast<int>(nonTerminal)] = nextState;
}

constexpr std::array<Terminal, 4> kFollowE = {
    Terminal::PLUS, Terminal::MINUS, Terminal::RPAREN, Terminal::END};
constexpr std::array<Terminal, 6> kFollowTF = {
    Terminal::PLUS, Terminal::MINUS, Terminal::STAR, Terminal::SLASH, Terminal::RPAREN,
    Terminal::END};

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

const std::array<std::array<Action, kNumTerminals>, kNumStates> ACTION_TABLE = [] {
    ActionTable table{};
    for (auto& row : table) {
        row.fill(errorAction());
    }

    setShift(table, 0, Terminal::ID, 5);
    setShift(table, 0, Terminal::LPAREN, 4);

    setShift(table, 1, Terminal::PLUS, 6);
    setShift(table, 1, Terminal::MINUS, 7);
    setAccept(table, 1, Terminal::END);

    setReduce(table, 2, kFollowE, 3);
    setShift(table, 2, Terminal::STAR, 8);
    setShift(table, 2, Terminal::SLASH, 9);

    setReduce(table, 3, kFollowTF, 6);

    setShift(table, 4, Terminal::ID, 5);
    setShift(table, 4, Terminal::LPAREN, 4);

    setReduce(table, 5, kFollowTF, 8);

    setShift(table, 6, Terminal::ID, 5);
    setShift(table, 6, Terminal::LPAREN, 4);

    setShift(table, 7, Terminal::ID, 5);
    setShift(table, 7, Terminal::LPAREN, 4);

    setShift(table, 8, Terminal::ID, 5);
    setShift(table, 8, Terminal::LPAREN, 4);

    setShift(table, 9, Terminal::ID, 5);
    setShift(table, 9, Terminal::LPAREN, 4);

    setShift(table, 10, Terminal::PLUS, 6);
    setShift(table, 10, Terminal::MINUS, 7);
    setShift(table, 10, Terminal::RPAREN, 15);

    setReduce(table, 11, kFollowE, 1);
    setShift(table, 11, Terminal::STAR, 8);
    setShift(table, 11, Terminal::SLASH, 9);

    setReduce(table, 12, kFollowE, 2);
    setShift(table, 12, Terminal::STAR, 8);
    setShift(table, 12, Terminal::SLASH, 9);

    setReduce(table, 13, kFollowTF, 4);

    setReduce(table, 14, kFollowTF, 5);
    setReduce(table, 15, kFollowTF, 7);

    return table;
}();

const std::array<std::array<int, kNumNonTerminals>, kNumStates> GOTO_TABLE = [] {
    GotoTable table{};
    for (auto& row : table) {
        row.fill(-1);
    }

    setGoto(table, 0, NonTerminal::E, 1);
    setGoto(table, 0, NonTerminal::T, 2);
    setGoto(table, 0, NonTerminal::F, 3);

    setGoto(table, 4, NonTerminal::E, 10);
    setGoto(table, 4, NonTerminal::T, 2);
    setGoto(table, 4, NonTerminal::F, 3);

    setGoto(table, 6, NonTerminal::T, 11);
    setGoto(table, 6, NonTerminal::F, 3);

    setGoto(table, 7, NonTerminal::T, 12);
    setGoto(table, 7, NonTerminal::F, 3);

    setGoto(table, 8, NonTerminal::F, 13);
    setGoto(table, 9, NonTerminal::F, 14);

    return table;
}();

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
