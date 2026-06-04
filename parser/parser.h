#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <stdexcept>
#include "../lexer/lexer.h"

enum class OpsType {
    OPERAND,
    OPERATION,
    LABEL
};

struct OpsElement {
    OpsType type;
    std::string value;
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;
    std::vector<OpsElement> ops;

    Token current() const;
    Token advance();
    void expect(TokenType type);
    void error(const std::string& msg) const;

    void addOperand(const std::string& val);
    void addOp(const std::string& op);
    void addLabelRef(int target);

    // Нерекурсивные методы для нетерминалов грамматики (рекурсивный спуск)
    void parsePROG();
    void parseSTMT_LIST();
    void parseSTMT_TAIL();
    void parseSTMT();
    void parseELSE_PART(int l1);
    void parseH();
    void parseCOND();
    std::string parseCMP();
    void parseS();
    void parseU();
    void parseT();
    void parseV();
    void parseF();

public:
    Parser(const std::vector<Token>& current_tokens);
    std::vector<OpsElement> parse();
    
    // Вспомогательный метод для отладки
    void printOps() const;
};

#endif // PARSER_H
