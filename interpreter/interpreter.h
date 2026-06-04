#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <string>
#include <map>
#include "../parser/parser.h"

class Interpreter {
private:
    std::vector<OpsElement> ops;
    std::map<std::string, double> variables;
    size_t ip; // Instruction Pointer

    bool isNumber(const std::string& str) const;
    double getValue(const std::string& str);

public:
    Interpreter(const std::vector<OpsElement>& ops_list);
    void execute();
};

#endif // INTERPRETER_H
