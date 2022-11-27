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

#include <SyntaxTree.h>

using namespace llvm;

class CodeGenerator {
public:
    IRBuilder<>* builder; // generating code
    LLVMContext TheContext; // core data structure of LLVM
    std::unique_ptr<Module> TheModule; // single cpp module
    std::map<std::string, Value*> NamedValues; // functions, variables, etc.
    SyntaxTree* ast;

    CodeGenerator(SyntaxTree* ast_);
    std::string CodeGen();

};


#endif //COMPILER_CODEGENERATOR_H
