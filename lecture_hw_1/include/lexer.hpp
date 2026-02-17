#pragma once

#include <string>

#include "token.hpp"

class Lexer {
public:
    explicit Lexer(std::string input);
    ~Lexer();

    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;

    Token nextToken();

private:
    std::string input_;
    void* buffer_;
    bool finished_;
};
