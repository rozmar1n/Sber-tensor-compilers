#include "parser.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "slr_table.hpp"
#include "token.hpp"

namespace {

std::string stackSymbolForToken(const Token& token) {
    switch (token.type) {
        case TokenType::ID:
            return token.lexeme;
        case TokenType::PLUS:
            return "+";
        case TokenType::MINUS:
            return "-";
        case TokenType::STAR:
            return "*";
        case TokenType::SLASH:
            return "/";
        case TokenType::LPAREN:
            return "(";
        case TokenType::RPAREN:
            return ")";
        case TokenType::END:
            return "$";
        case TokenType::INVALID:
            return token.lexeme;
    }
    return "?";
}

}  // namespace

Parser::Parser(Lexer& lexer) {
    while (true) {
        Token token = lexer.nextToken();
        tokens_.push_back(token);
        if (token.type == TokenType::END) {
            break;
        }
    }
}

bool Parser::parse(bool trace) {
    std::vector<StackEntry> stack;
    stack.push_back({"$", 0});

    std::size_t tokenPosition = 0;

    if (trace) {
        std::cout << std::left << std::setw(38) << "STACK"
                  << " | " << std::setw(34) << "INPUT"
                  << " | ACTION\n";
    }

    while (true) {
        const int state = stack.back().state;
        const Token& lookahead = tokens_.at(tokenPosition);
        const Action action = actionFor(state, lookahead.type);

        if (trace) {
            std::cout << std::left << std::setw(38) << stackToString(stack)
                      << " | " << std::setw(34) << inputToString(tokenPosition)
                      << " | " << actionToString(action) << '\n';
        }

        if (action.type == ActionType::SHIFT) {
            stack.push_back({stackSymbolForToken(lookahead), action.value});
            ++tokenPosition;
            continue;
        }

        if (action.type == ActionType::REDUCE) {
            if (action.value <= 0 || action.value >= kNumProductions) {
                return false;
            }

            const ProductionInfo& production = PRODUCTIONS[action.value];
            for (int i = 0; i < production.rhsLength; ++i) {
                if (stack.size() <= 1) {
                    return false;
                }
                stack.pop_back();
            }

            const int fromState = stack.back().state;
            const int nextState = gotoState(fromState, nonTerminalIndex(production.lhs));
            if (nextState < 0) {
                return false;
            }
            stack.push_back({production.lhsName, nextState});
            continue;
        }

        if (action.type == ActionType::ACCEPT) {
            return true;
        }

        if (trace && lookahead.type == TokenType::INVALID) {
            std::cout << "Lexer error: invalid token '" << lookahead.lexeme << "'\n";
        }
        return false;
    }
}

std::string Parser::stackToString(const std::vector<StackEntry>& stack) const {
    std::ostringstream out;
    for (std::size_t i = 0; i < stack.size(); ++i) {
        if (i > 0) {
            out << ' ';
        }
        out << stack[i].symbol << ' ' << stack[i].state;
    }
    return out.str();
}

std::string Parser::inputToString(std::size_t tokenPosition) const {
    std::ostringstream out;
    for (std::size_t i = tokenPosition; i < tokens_.size(); ++i) {
        out << tokenForInputView(tokens_[i]);
        if (i + 1 < tokens_.size()) {
            out << ' ';
        }
    }
    return out.str();
}
