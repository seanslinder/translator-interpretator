#ifndef LEXER_H
#define LEXER_H

#include <string>

enum TokenType {
    // Служебные слова
    TOKEN_BEGIN = 1,
    TOKEN_END = 2,
    TOKEN_IF = 3,
    TOKEN_THEN = 4,
    TOKEN_ELSE = 5,
    TOKEN_WHILE = 6,
    TOKEN_DO = 7,
    TOKEN_READ = 8,
    TOKEN_WRITE = 9,
    // Встроенные функции
    TOKEN_SQRT = 10,
    TOKEN_EXP = 11,
    TOKEN_LOG = 12,
    // Идентификаторы и литералы
    TOKEN_ID = 20,
    TOKEN_INT = 21,
    TOKEN_REAL = 22,
    // Операторы
    TOKEN_ASSIGN = 30,
    TOKEN_PLUS = 31,
    TOKEN_MINUS = 32,
    TOKEN_MUL = 33,
    TOKEN_DIV = 34,
    // Сравнения
    TOKEN_EQ = 40,
    TOKEN_NEQ = 41,
    TOKEN_LESS = 42,
    TOKEN_LEQ = 43,
    TOKEN_GREATER = 44,
    TOKEN_GEQ = 45,
    // Разделители
    TOKEN_LPAREN = 50,
    TOKEN_RPAREN = 51,
    TOKEN_LBRACKET = 52,
    TOKEN_RBRACKET = 53,
    TOKEN_SEMI = 54,
    // Специальные
    TOKEN_EOF = 99,
    TOKEN_ERROR = 100
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int pos;
};

class Lexer {
private:
    std::string input;
    int pos;
    int line;
    int col;

    char peek();
    char advance();
    void skipWhitespace();
    TokenType checkKeywordOrId(const std::string& text);

public:
    Lexer(std::string src);
    Token nextToken();
};

#endif