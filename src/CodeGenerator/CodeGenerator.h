//
// Created by Neko Toxin on 2022/11/23.
//

#ifndef COMPILER_CODEGENERATOR_H
#define COMPILER_CODEGENERATOR_H

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include <AST.h>
#include <vector>

using namespace llvm;

// myerror inherits from llvm error class
class MyError : public llvm::ErrorInfo<MyError> {
public:
    static char ID;
    std::string msg;

    MyError(std::string msg) : msg(std::move(msg)) {}

    void log(raw_ostream &OS) const override {
        OS << msg;
    }

    std::error_code convertToErrorCode() const override {
        return llvm::inconvertibleErrorCode();
    }
};

class SymbolTable {
public:
    typedef std::map<std::string, Value*> m_SymbolTable; // functions, variables, etc.
    std::vector<m_SymbolTable> symbol_table_stack; // stack of symbol tables

    SymbolTable() {
        symbol_table_stack.push_back(m_SymbolTable());
    }

    Value* getSymbol(std::string name) {
        for (auto it = symbol_table_stack.rbegin(); it != symbol_table_stack.rend(); it++) {
            auto symbol = it->find(name);
            if (symbol != it->end()) {
                return symbol->second;
            }
        }
        return nullptr;
    }

    int isConflict(std::string name) {
        return symbol_table_stack.back().find(name) != symbol_table_stack.back().end();
    }

    int addSymbol(std::string name, Value* value) {
        symbol_table_stack.back()[name] = value;
    }

    void beginScope() {
        symbol_table_stack.emplace_back();
    }

    int endScope() {
        symbol_table_stack.pop_back();
    }

};

class CodeGenerator {
public:
    IRBuilder<>* builder; // generating code
    LLVMContext the_context; // core data structure of LLVM
    std::unique_ptr<Module> the_module; // single cpp module
//    std::map<std::string, Value*> named_values; // functions, variables, etc.
    SymbolTable symbol_table; // symbol table
    AST* ast;

    std::vector<std::string> errors;

    CodeGenerator(AST* ast_);
    std::string CodeGen();
    Value * CodeGenHelper(ASTNode* node);
    Function* CodeGenFunc(ASTNode *node);
    AllocaInst* CreateBlockAlloca(Function* func, const std::string& name) ;
    void logError(const std::string& prompt);

};


#endif //COMPILER_CODEGENERATOR_H
