#include "lexer.h"
#include <cctype>

Lexer::Lexer(std::string src) : input(src), pos(0), line(1), col(1) {}

char Lexer::peek() {
    if (pos >= input.length()) return '\0';
    return input[pos];
}

char Lexer::advance() {
    char c = peek();
    if (c != '\0') {
        pos++;
        col++;
        if (c == '\n') {
            line++;
            col = 1;
        }
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

TokenType Lexer::checkKeywordOrId(const std::string& text) {
    if (text == "begin") return TOKEN_BEGIN;
    if (text == "end") return TOKEN_END;
    if (text == "if") return TOKEN_IF;
    if (text == "then") return TOKEN_THEN;
    if (text == "else") return TOKEN_ELSE;
    if (text == "while") return TOKEN_WHILE;
    if (text == "do") return TOKEN_DO;
    if (text == "read") return TOKEN_READ;
    if (text == "write") return TOKEN_WRITE;
    if (text == "sqrt") return TOKEN_SQRT;
    if (text == "exp") return TOKEN_EXP;
    if (text == "log") return TOKEN_LOG;
    return TOKEN_ID;
}

Token Lexer::nextToken() {
    skipWhitespace();

    if (peek() == '\0') {
        return {TOKEN_EOF, "EOF", line, col};
    }

    int startCol = col;
    std::string text = "";
    int state = 0;

    while (true) {
        char c = peek();

        switch (state) {
            case 0: // S0
                if (isalpha(c)) {
                    text += advance();
                    state = 1;
                } else if (isdigit(c)) {
                    text += advance();
                    state = 2;
                } else if (c == '.') {
                    text += advance();
                    return {TOKEN_ERROR, text, line, startCol};
                } else if (c == ':') {
                    text += advance();
                    state = 5;
                } else if (c == '=') {
                    text += advance();
                    return {TOKEN_EQ, text, line, startCol};
                } else if (c == '<') {
                    text += advance();
                    state = 6;
                } else if (c == '>') {
                    text += advance();
                    state = 7;
                } else if (c == '+' || c == '-' || c == '*' || c == '/' || 
                           c == '(' || c == ')' || c == '[' || c == ']' || 
                           c == ';') {
                    text += advance();
                    TokenType type = TOKEN_ERROR;
                    if (c == '+') type = TOKEN_PLUS;
                    if (c == '-') type = TOKEN_MINUS;
                    if (c == '*') type = TOKEN_MUL;
                    if (c == '/') type = TOKEN_DIV;
                    if (c == '(') type = TOKEN_LPAREN;
                    if (c == ')') type = TOKEN_RPAREN;
                    if (c == '[') type = TOKEN_LBRACKET;
                    if (c == ']') type = TOKEN_RBRACKET;
                    if (c == ';') type = TOKEN_SEMI;
                    return {type, text, line, startCol};
                } else if (c == '\0') {
                    return {TOKEN_EOF, "EOF", line, startCol};
                } else { // Другое
                    text += advance();
                    return {TOKEN_ERROR, text, line, startCol};
                }
                break;

            case 1: // S1: Идентификатор/Служебное слово
                if (isalnum(c)) {
                    text += advance();
                } else {
                    return {checkKeywordOrId(text), text, line, startCol};
                }
                break;

            case 2: // S2: Целое число
                if (isdigit(c)) {
                    text += advance();
                } else if (c == '.') {
                    text += advance();
                    state = 3;
                } else {
                    return {TOKEN_INT, text, line, startCol};
                }
                break;

            case 3: // S3: Точка
                if (isdigit(c)) {
                    text += advance();
                    state = 4;
                } else {
                    return {TOKEN_ERROR, text, line, startCol};
                }
                break;

            case 4: // S4: Вещественное число
                if (isdigit(c)) {
                    text += advance();
                } else {
                    return {TOKEN_REAL, text, line, startCol};
                }
                break;

            case 5: // S5: Двоеточие
                if (c == '=') {
                    text += advance();
                    return {TOKEN_ASSIGN, text, line, startCol};
                } else {
                    return {TOKEN_ERROR, text, line, startCol};
                }
                break;

            case 6: // S6: Меньше <
                if (c == '=') {
                    text += advance();
                    return {TOKEN_LEQ, text, line, startCol};
                } else if (c == '>') {
                    text += advance();
                    return {TOKEN_NEQ, text, line, startCol};
                } else {
                    return {TOKEN_LESS, text, line, startCol};
                }
                break;

            case 7: // S7: Больше >
                if (c == '=') {
                    text += advance();
                    return {TOKEN_GEQ, text, line, startCol};
                } else {
                    return {TOKEN_GREATER, text, line, startCol};
                }
                break;
        }
    }
}