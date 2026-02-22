#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "lexer.hpp"

class Parser {
public:
    explicit Parser(Lexer& lexer);

    bool parse(bool trace = true);

private:
    struct StackEntry {
        std::string symbol;
        int state;
    };

    std::vector<Token> tokens_;

    std::string stackToString(const std::vector<StackEntry>& stack) const;
    std::string inputToString(std::size_t tokenPosition) const;
};
