#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"

int main(int argc, char** argv) {
    bool verbose = false; // Включить подробный вывод для отладки
    // По умолчанию читаем из lexer/code.txt
    std::string filename = "tests/code.txt";
    if (argc > 1) {
        filename = argv[1];
    }
    if (argc > 2 && std::string(argv[2]) == "--verbose") {
        verbose = true;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        if (verbose) {
            std::cout << "=== 1. ЛЕКСИЧЕСКИЙ АНАЛИЗ ===" << std::endl;
        }
        Lexer lexer(source);
        std::vector<Token> tokens;
        Token t = lexer.nextToken();
        
        while (t.type != TOKEN_EOF && t.type != TOKEN_ERROR) {
            tokens.push_back(t);
            t = lexer.nextToken();
        }
        if (t.type == TOKEN_ERROR) {
            std::cerr << "Лексическая ошибка в строке " << t.line << ", позиция " << t.pos << std::endl;
            return 1;
        }
        if (verbose) {
            std::cout << "Лексический анализ успешно завершен. Токенов: " << tokens.size() << std::endl << std::endl;
        }
        if  (verbose){
            std::cout << "=== 2. СИНТАКСИЧЕСКИЙ АНАЛИЗ И ГЕНЕРАЦИЯ ОПС ===" << std::endl;
        }
        Parser parser(tokens);
        std::vector<OpsElement> ops = parser.parse();

        // Вывод сгенерированной ОПС для отладки
        if (verbose) {
            parser.printOps();
            std::cout << "Генерация ОПС успешно завершена." << std::endl << std::endl;
        }
        
        
        if (verbose) {
            std::cout << "=== 3. ИСПОЛНЕНИЕ (ИНТЕРПРЕТАТОР ОПС) ===" << std::endl;
        }
        Interpreter interpreter(ops);
        interpreter.execute();
        
        if (verbose) {
            std::cout << std::endl << "Программа успешно завершена." << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "ОШИБКА: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
