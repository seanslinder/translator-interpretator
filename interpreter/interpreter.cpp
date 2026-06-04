#include "interpreter.h"
#include <stack>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <cctype>

using namespace std;

Interpreter::Interpreter(const std::vector<OpsElement>& ops_list) : ops(ops_list), ip(0) {}

bool Interpreter::isNumber(const std::string& str) const {
    if (str.empty()) return false;
    // Проверка, является ли строка числом (с учетом возможного минуса)
    return isdigit(str[0]) || (str[0] == '-' && str.size() > 1 && isdigit(str[1]));
}

double Interpreter::getValue(const std::string& str) {
    if (isNumber(str)) {
        return std::stod(str);
    }
    // Иначе это переменная (операнд типа ID или элемент массива ID[idx])
    return variables[str]; 
}

void Interpreter::execute() {
    std::stack<std::string> st;

    while (ip < ops.size()) {
        const OpsElement& elem = ops[ip];

        if (elem.type == OpsType::OPERAND || elem.type == OpsType::LABEL) {
            // Кладем операнд или метку на стек
            st.push(elem.value);
            ip++;
        } else if (elem.type == OpsType::OPERATION) {
            std::string op = elem.value;

            // Бинарные арифметические операции и сравнения
            if (op == "+" || op == "-" || op == "*" || op == "/" ||
                op == "=" || op == "<>" || op == "<" || op == "<=" || op == ">" || op == ">=") {
                if (st.size() < 2) throw std::runtime_error("Stack underflow on binary operation " + op);
                double right = getValue(st.top()); st.pop();
                double left = getValue(st.top()); st.pop();
                double res = 0.0;

                if (op == "+") res = left + right;
                else if (op == "-") res = left - right;
                else if (op == "*") res = left * right;
                else if (op == "/") res = left / right;
                else if (op == "=") res = (left == right) ? 1.0 : 0.0;
                else if (op == "<>") res = (left != right) ? 1.0 : 0.0;
                else if (op == "<") res = (left < right) ? 1.0 : 0.0;
                else if (op == "<=") res = (left <= right) ? 1.0 : 0.0;
                else if (op == ">") res = (left > right) ? 1.0 : 0.0;
                else if (op == ">=") res = (left >= right) ? 1.0 : 0.0;

                // Кладем результат обратно на стек как строку
                st.push(std::to_string(res));
                ip++;
            } 
            // Унарные математические функции
            else if (op == "sqrt_op" || op == "exp_op" || op == "log_op") {
                if (st.empty()) throw std::runtime_error("Stack underflow on unary operation " + op);
                double val = getValue(st.top()); st.pop();
                double res = 0.0;

                if (op == "sqrt_op") res = std::sqrt(val);
                else if (op == "exp_op") res = std::exp(val);
                else if (op == "log_op") res = std::log(val);

                st.push(std::to_string(res));
                ip++;
            } 
            // Операция присваивания
            else if (op == ":=") {
                if (st.size() < 2) throw std::runtime_error("Stack underflow on :=");
                double right = getValue(st.top()); st.pop();
                std::string left = st.top(); st.pop(); // left - имя переменной или массива
                variables[left] = right;
                ip++;
            } 
            // Индексация элемента массива (напр. 'a', '5' -> 'a[5]')
            else if (op == "i") {
                if (st.size() < 2) throw std::runtime_error("Stack underflow on array indexing");
                double idx = getValue(st.top()); st.pop();
                std::string array_name = st.top(); st.pop();
                
                // Формируем уникальное имя "array[index]" и кладем его на стек
                st.push(array_name + "[" + std::to_string((int)idx) + "]");
                ip++;
            } 
            // Безусловный переход
            else if (op == "j") {
                if (st.empty()) throw std::runtime_error("Stack underflow on j");
                int label = std::stoi(st.top()); st.pop();
                ip = label; // Изменяем Instruction Pointer (IP)
            } 
            // Переход по лжи
            else if (op == "jf") {
                if (st.size() < 2) throw std::runtime_error("Stack underflow on jf");
                int label = std::stoi(st.top()); st.pop();
                double cond = getValue(st.top()); st.pop();
                
                if (cond == 0.0) { // Ложь
                    ip = label;
                } else {
                    ip++;
                }
            } 
            // Ввод с клавиатуры (read)
            else if (op == "r") {
                if (st.empty()) throw std::runtime_error("Stack underflow on r");
                std::string name = st.top(); st.pop();
                double input_val;
                std::cout << "> Введите значение для " << name << ": ";
                std::cin >> input_val;
                variables[name] = input_val;
                ip++;
            } 
            // Вывод на экран (write)
            else if (op == "w") {
                if (st.empty()) throw std::runtime_error("Stack underflow on w");
                double val = getValue(st.top()); st.pop();
                std::cout << val << std::endl;
                ip++;
            } 
            else {
                throw std::runtime_error("Unknown operation: " + op);
            }
        }
    }
}
