#include "lexer.hpp"

#include <stdexcept>
#include <utility>

struct yy_buffer_state;
using YY_BUFFER_STATE = yy_buffer_state*;

int yylex();
YY_BUFFER_STATE yy_scan_string(const char* yyStr);
void yy_delete_buffer(YY_BUFFER_STATE buffer);
int yylex_destroy();

extern Token g_scanned_token;

Lexer::Lexer(std::string input)
    : input_(std::move(input)),
      buffer_(nullptr),
      finished_(false) {
    buffer_ = static_cast<void*>(yy_scan_string(input_.c_str()));
    if (buffer_ == nullptr) {
        throw std::runtime_error("Failed to initialize lexer buffer.");
    }
}

Lexer::~Lexer() {
    if (buffer_ != nullptr) {
        yy_delete_buffer(static_cast<YY_BUFFER_STATE>(buffer_));
        buffer_ = nullptr;
    }
    yylex_destroy();
}

Token Lexer::nextToken() {
    if (finished_) {
        return Token{TokenType::END, "$"};
    }

    const int result = yylex();
    if (result == 0) {
        finished_ = true;
        return Token{TokenType::END, "$"};
    }
    return g_scanned_token;
}
