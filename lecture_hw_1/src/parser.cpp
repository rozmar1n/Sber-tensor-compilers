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
    Stack stack;
    stack.push_back({"$", 0});

    std::size_t tokenPosition = 0;

    if (trace) {
        printTraceHeader();
    }

    while (true) {
        const int state = stack.back().state;
        const Token& lookahead = tokens_.at(tokenPosition);
        const Action action = actionFor(state, lookahead.type);

        if (trace) {
            printTraceStep(stack, tokenPosition, action);
        }

        switch (action.type) {
            case ActionType::SHIFT:
                if (!applyShift(lookahead, action, stack, tokenPosition)) {
                    return false;
                }
                break;
            case ActionType::REDUCE:
                if (!applyReduce(action, stack)) {
                    return false;
                }
                break;
            case ActionType::ACCEPT:
                return true;
            case ActionType::ERROR:
                if (!handleError(lookahead, trace)) {
                    return false;
                }
                break;
            default:
                return false;
        }
    }
}

void Parser::printTraceHeader() const {
    std::cout << std::left << std::setw(38) << "STACK"
              << " | " << std::setw(34) << "INPUT"
              << " | ACTION\n";
}

void Parser::printTraceStep(const Stack& stack, std::size_t tokenPosition, Action action) const {
    std::cout << std::left << std::setw(38) << stackToString(stack)
              << " | " << std::setw(34) << inputToString(tokenPosition)
              << " | " << actionToString(action) << '\n';
}

bool Parser::applyShift(const Token& lookahead, Action action, Stack& stack,
                        std::size_t& tokenPosition) const {
    stack.push_back({stackSymbolForToken(lookahead), action.value});
    ++tokenPosition;
    return true;
}

bool Parser::applyReduce(Action action, Stack& stack) const {
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
    return true;
}

bool Parser::handleError(const Token& lookahead, bool trace) const {
    if (trace && lookahead.type == TokenType::INVALID) {
        std::cout << "Lexer error: invalid token '" << lookahead.lexeme << "'\n";
    }
    return false;
}

std::string Parser::stackToString(const Stack& stack) const {
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
