//
// Created by Neko Toxin on 2022/10/6.
//

#include <iomanip>
#include <stack>
#include "Symbol.h"

// put var symbol
void SymbolTable::PutSymbol(const std::string &symbol_name, type symbol_type) {
    // check if the symbol is already in current scope
    if (symbol_table_stack.back()->find(symbol_name) != symbol_table_stack.back()->end()) {
        std::cerr << "Error: symbol " << symbol_name << " already defined in current scope" << std::endl;
    }
    (*symbol_table_stack.back())[symbol_name] = symbol_type;
    std::cout << "Put symbol " << symbol_name << " in current scope" << std::endl;
}

void SymbolTable::PutSymbol(const std::string &symbol_name, type return_type, const std::vector<type> &param_type) {
    // check if the symbol is already in current scope
    if (symbol_table_stack.back()->find(symbol_name) != symbol_table_stack.back()->end()) {
        std::cerr << "Error: symbol " << symbol_name << " already defined in current scope" << std::endl;
    }
    (*symbol_table_stack.back())[symbol_name] = FUNC;
    func_id_table[symbol_name] = {symbol_name, return_type, param_type, 0};
    std::cout << "Put symbol " << symbol_name << " in current scope" << std::endl;
}

void SymbolTable::BeginScope(){
    symbol_table_stack.push_back(new _SymbolTable());
    std::cout << "Begin a new scope" << std::endl;
}

void SymbolTable::EndScope() {
    if (symbol_table_stack.size() == 1) {
        std::cerr << "Error: cannot end global scope" << std::endl;
    }
    symbol_table_stack.pop_back();
    std::cout << "End current scope" << std::endl;
}

VarId SymbolTable::GetVar(const std::string &var_name) {
    for (auto it = symbol_table_stack.rbegin(); it != symbol_table_stack.rend(); it++) {
        if ((*it)->find(var_name) != (*it)->end()&&(*it)->at(var_name)!=FUNC) {
            VarId var_id = {var_name, (*it)->at(var_name), (int)std::distance(symbol_table_stack.rbegin(), it)};
            return var_id;
        }
    }
    std::cerr << "Error: symbol " << var_name << " not defined" << std::endl;
    return {var_name, ERROR, -1};
}

FuncId SymbolTable::GetFunc(const std::string &func_name) {
    if (func_id_table.find(func_name) != func_id_table.end()) {
        return func_id_table[func_name];
    }
    std::cerr << "Error: symbol " << func_name << " not defined" << std::endl;
    return {func_name, ERROR, {}, -1};
}




