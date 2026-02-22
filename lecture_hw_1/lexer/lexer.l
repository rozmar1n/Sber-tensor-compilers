%option noyywrap nounput noinput nodefault

%{
#include "token.hpp"

Token g_scanned_token{TokenType::END, "$"};
%}

WHITESPACE [ \t\r\n]+
NUMBER     [0-9]+
IDENTIFIER [a-zA-Z]+

%%
{WHITESPACE}   { }
{NUMBER}       { g_scanned_token = Token{TokenType::ID,         yytext};    return 1; }
{IDENTIFIER}   { g_scanned_token = Token{TokenType::ID,         yytext};    return 1; }
"+"            { g_scanned_token = Token{TokenType::PLUS,       yytext};    return 1; }
"-"            { g_scanned_token = Token{TokenType::MINUS,      yytext};    return 1; }
"*"            { g_scanned_token = Token{TokenType::STAR,       yytext};    return 1; }
"/"            { g_scanned_token = Token{TokenType::SLASH,      yytext};    return 1; }
"("            { g_scanned_token = Token{TokenType::LPAREN,     yytext};    return 1; }
")"            { g_scanned_token = Token{TokenType::RPAREN,     yytext};    return 1; }
<<EOF>>        { g_scanned_token = Token{TokenType::END,        "$"};       return 0; }
.              { g_scanned_token = Token{TokenType::INVALID,    yytext};    return 1; }
%%
