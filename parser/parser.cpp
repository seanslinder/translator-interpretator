#include "parser.h"
#include <iostream>

using namespace std;

Parser::Parser(const std::vector<Token>& current_tokens) : tokens(current_tokens), pos(0) {}

Token Parser::current() const {
    if (pos < tokens.size()) {
        return tokens[pos];
    }
    // Возвращаем токен конца файла, если вышли за границы
    Token eof;
    eof.type = TOKEN_EOF;
    eof.value = "";
    eof.line = tokens.empty() ? 0 : tokens.back().line;
    return eof;
}

Token Parser::advance() {
    Token curr = current();
    if (pos < tokens.size()) {
        pos++;
    }
    return curr;
}

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TOKEN_BEGIN:   return "'begin'";
        case TOKEN_END:     return "'end'";
        case TOKEN_IF:      return "'if'";
        case TOKEN_THEN:    return "'then'";
        case TOKEN_ELSE:    return "'else'";
        case TOKEN_WHILE:   return "'while'";
        case TOKEN_DO:      return "'do'";
        case TOKEN_READ:    return "'read'";
        case TOKEN_WRITE:   return "'write'";
        case TOKEN_SQRT:    return "'sqrt'";
        case TOKEN_EXP:     return "'exp'";
        case TOKEN_LOG:     return "'log'";
        case TOKEN_ID:      return "identifier";
        case TOKEN_INT:     return "integer";
        case TOKEN_REAL:    return "real number";
        case TOKEN_ASSIGN:  return "':='";
        case TOKEN_PLUS:    return "'+'";
        case TOKEN_MINUS:   return "'-'";
        case TOKEN_MUL:     return "'*'";
        case TOKEN_DIV:     return "'/'";
        case TOKEN_EQ:      return "'='";
        case TOKEN_NEQ:     return "'<>'";
        case TOKEN_LESS:    return "'<'";
        case TOKEN_LEQ:     return "'<='";
        case TOKEN_GREATER: return "'>'";
        case TOKEN_GEQ:     return "'>='";
        case TOKEN_LPAREN:  return "'('";
        case TOKEN_RPAREN:  return "')'";
        case TOKEN_LBRACKET:return "'['";
        case TOKEN_RBRACKET:return "']'";
        case TOKEN_SEMI:    return "';'";
        case TOKEN_EOF:     return "end of file";
        case TOKEN_ERROR:   return "error token";
        default:            return "unknown token";
    }
}

void Parser::expect(TokenType type) {
    if (current().type == type) {
        advance();
    } else {
        std::string expectedStr = tokenTypeToString(type);
        std::string gotStr = tokenTypeToString(current().type);
        // if (!current().value.empty()) {
        //     gotStr += " (\"" + current().value + "\")";
        // }
        error("Unexpected token. Expected " + expectedStr + ", got " + gotStr);
    }
}

void Parser::error(const std::string& msg) const {
    throw std::runtime_error("Syntax error at line " + std::to_string(current().line) + ", pos " + std::to_string(current().pos) + ": " + msg);
}

void Parser::addOperand(const std::string& val) {
    ops.push_back({OpsType::OPERAND, val});
}

void Parser::addOp(const std::string& op) {
    ops.push_back({OpsType::OPERATION, op});
}

void Parser::addLabelRef(int target) {
    if (target < 0) {
        ops.push_back({OpsType::LABEL, ""}); // Placeholder, будет разрешен позже
    } else {
        ops.push_back({OpsType::LABEL, std::to_string(target)});
    }
}

std::vector<OpsElement> Parser::parse() {
    ops.clear();
    pos = 0;
    parsePROG();
    return ops;
}

void Parser::parsePROG() {
    expect(TOKEN_BEGIN);
    parseSTMT_LIST();
    expect(TOKEN_END);
}

void Parser::parseSTMT_LIST() {
    parseSTMT();
    parseSTMT_TAIL();
}

void Parser::parseSTMT_TAIL() {
    if (current().type == TOKEN_SEMI) {
        advance();
        parseSTMT();
        parseSTMT_TAIL();
    }
    // else λ - ничего не делаем
}

void Parser::parseSTMT() {
    TokenType t = current().type;
    if (t == TOKEN_ID) {
        // a H := S
        std::string id = current().value;
        advance();
        addOperand(id);
        
        parseH();
        expect(TOKEN_ASSIGN);
        parseS();
        
        addOp(":=");
    } else if (t == TOKEN_IF) {
        // if COND then STMT ELSE_PART
        advance();
        parseCOND();
        expect(TOKEN_THEN);
        
        // Action 1: метка для перехода при условном переходе по лжи (jf)
        int l1 = ops.size();
        addLabelRef(-1);
        addOp("jf");
        
        parseSTMT();
        parseELSE_PART(l1);
    } else if (t == TOKEN_WHILE) {
        // while COND do STMT
        advance();
        
        // Action 4: запоминаем стартовый адрес цикла
        int loop_start = ops.size();
        
        parseCOND();
        expect(TOKEN_DO);
        
        // Action 1: метка для перехода из цикла, если условие ложно
        int l1 = ops.size();
        addLabelRef(-1);
        addOp("jf");
        
        parseSTMT();
        
        // Action 5: безусловный прыжок в начало цикла
        addLabelRef(loop_start);
        addOp("j");
        
        // Разрешение метки l1: указывает на код после всего цикла
        ops[l1].value = std::to_string(ops.size());
    } else if (t == TOKEN_READ) {
        // read ( a H )
        advance();
        expect(TOKEN_LPAREN);
        
        if (current().type != TOKEN_ID) {
            error("Expected identifier in read statement");
        }
        std::string id = current().value;
        advance();
        addOperand(id);
        
        parseH();
        expect(TOKEN_RPAREN);
        
        addOp("r");
    } else if (t == TOKEN_WRITE) {
        // write ( S )
        advance();
        expect(TOKEN_LPAREN);
        parseS();
        expect(TOKEN_RPAREN);
        
        addOp("w");
    } else if (t == TOKEN_BEGIN) {
        // begin STMT_LIST end
        advance();
        parseSTMT_LIST();
        expect(TOKEN_END);
    } else {
        error("Unexpected token in statement");
    }
}

void Parser::parseELSE_PART(int l1) {
    if (current().type == TOKEN_ELSE) {
        // else STMT
        advance();
        
        // Action 2: безусловный прыжок в конец условной конструкции
        int l2 = ops.size();
        addLabelRef(-1);
        addOp("j");
        
        // Разрешаем l1 указывать на начало блока else
        ops[l1].value = std::to_string(ops.size());
        
        parseSTMT();
        
        // Action 3: разрешаем l2 указывать на конец всей конструкции if/else
        ops[l2].value = std::to_string(ops.size());
    } else {
        // λ
        // Action 3: ветки else нет, тогда ложное условие прыгает сразу в конец if
        ops[l1].value = std::to_string(ops.size());
    }
}

void Parser::parseH() {
    if (current().type == TOKEN_LBRACKET) {
        advance();
        parseS();
        expect(TOKEN_RBRACKET);
        addOp("i");
    }
    // else λ - ничего не делаем
}

void Parser::parseCOND() {
    parseS();
    std::string cmp_op = parseCMP();
    parseS();
    addOp(cmp_op);
}

std::string Parser::parseCMP() {
    std::string op = "";
    TokenType t = current().type;
    if (t == TOKEN_EQ) op = "=";
    else if (t == TOKEN_NEQ) op = "<>";
    else if (t == TOKEN_LESS) op = "<";
    else if (t == TOKEN_LEQ) op = "<=";
    else if (t == TOKEN_GREATER) op = ">";
    else if (t == TOKEN_GEQ) op = ">=";
    else error("Expected comparison operator");
    
    advance();
    return op;
}

void Parser::parseS() {
    // S -> ( S ) V U | a H V U | k V U | sqrt(S) V U | exp(S) V U | log(S) V U 
    parseT();
    parseU();
}

void Parser::parseU() {
    // U -> + T U | - T U | λ
    TokenType t = current().type;
    if (t == TOKEN_PLUS || t == TOKEN_MINUS) {
        advance();
        parseT();
        parseU();
        addOp(t == TOKEN_PLUS ? "+" : "-");
    }
    // else λ
}

void Parser::parseT() {
    // T -> F V
    parseF();
    parseV();
}

void Parser::parseV() {
    // V -> * F V | / F V | λ
    TokenType t = current().type;
    if (t == TOKEN_MUL || t == TOKEN_DIV) {
        advance();
        parseF();
        parseV();
        addOp(t == TOKEN_MUL ? "*" : "/");
    }
    // else λ
}

void Parser::parseF() {
    // F -> ( S ) | a H | k | sqrt(S) | exp(S) | log(S)
    TokenType t = current().type;
    if (t == TOKEN_LPAREN) {
        advance();
        parseS();
        expect(TOKEN_RPAREN);
    } else if (t == TOKEN_ID) {
        std::string id = current().value;
        advance();
        addOperand(id);
        parseH();
    } else if (t == TOKEN_INT || t == TOKEN_REAL) {
        std::string val = current().value;
        advance();
        addOperand(val);
    } else if (t == TOKEN_SQRT || t == TOKEN_EXP || t == TOKEN_LOG) {
        advance();
        expect(TOKEN_LPAREN);
        parseS();
        expect(TOKEN_RPAREN);
        if (t == TOKEN_SQRT) addOp("sqrt_op");
        else if (t == TOKEN_EXP) addOp("exp_op");
        else if (t == TOKEN_LOG) addOp("log_op");
    } else {
        error("Unexpected token in expression (Expected ID, Literal, (, or Function)");
    }
}

void Parser::printOps() const {
    cout << "ОПС: ";
    for (size_t i = 0; i < ops.size(); ++i) {
        if (ops[i].type == OpsType::LABEL) {
            cout << "LABEL[" << ops[i].value << "]";
        } else {
            cout << ops[i].value << " ";
        }
    }
    cout << endl;
    for (size_t i = 0; i < ops.size(); ++i) {
        cout << i << ": ";
        if (ops[i].type == OpsType::OPERAND) {
            cout << "OP_ND[" << ops[i].value << "]";
        } else if (ops[i].type == OpsType::OPERATION) {
            cout << "OP_OP[" << ops[i].value << "]";
        } else if (ops[i].type == OpsType::LABEL) {
            cout << "LABEL[" << ops[i].value << "]";
        }
        cout << endl;
    }
}
