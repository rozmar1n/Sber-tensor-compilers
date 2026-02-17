#pragma once

#include <string>

enum class TokenType {
    ID,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    LPAREN,
    RPAREN,
    END,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
};

inline std::string tokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::ID:
            return "ID";
        case TokenType::PLUS:
            return "PLUS";
        case TokenType::MINUS:
            return "MINUS";
        case TokenType::STAR:
            return "STAR";
        case TokenType::SLASH:
            return "SLASH";
        case TokenType::LPAREN:
            return "LPAREN";
        case TokenType::RPAREN:
            return "RPAREN";
        case TokenType::END:
            return "END";
        case TokenType::INVALID:
            return "INVALID";
    }
    return "INVALID";
}

inline std::string tokenForInputView(const Token& token) {
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
