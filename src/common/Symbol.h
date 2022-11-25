#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <vector>

typedef enum {
    INT, FLOAT, VOID, FUNC , ERROR
} type;

typedef struct {
    std::string name;
    type var_type;
    int scope;
} VarId;

// function identifier
typedef struct {
    std::string name;
    type return_type;
    std::vector<type> param_type;
    int scope;
} FuncId;


class SymbolTable {
public:
    typedef std::map<std::string, type> _SymbolTable;
    std::vector<_SymbolTable*> symbol_table_stack;
    std::map<std::string, FuncId> func_id_table;

    SymbolTable() {
        symbol_table_stack.clear();
        func_id_table.clear();

        // the global, init first symbol table
        symbol_table_stack.push_back(new _SymbolTable());
    }

    void PutSymbol(const std::string &symbol_name, type symbol_type);

    void PutSymbol(const std::string &symbol_name, type return_type, const std::vector<type> &param_type);

    VarId GetVar(const std::string &var_name);

    FuncId GetFunc(const std::string &func_name);

    void BeginScope();

    void EndScope();


};



