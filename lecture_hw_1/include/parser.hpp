#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "slr_table.hpp"

class Parser {
public:
    explicit Parser(Lexer& lexer);

    bool parse(bool trace = true);

private:
    struct StackEntry {
        std::string symbol;
        int state;
    };

    using Stack = std::vector<StackEntry>;

    std::vector<Token> tokens_;

    void printTraceHeader() const;
    void printTraceStep(const Stack& stack, std::size_t tokenPosition, Action action) const;

    bool applyShift(const Token& lookahead, Action action, Stack& stack, std::size_t& tokenPosition) const;
    bool applyReduce(Action action, Stack& stack) const;
    bool handleError(const Token& lookahead, bool trace) const;

    std::string stackToString(const Stack& stack) const;
    std::string inputToString(std::size_t tokenPosition) const;
};
