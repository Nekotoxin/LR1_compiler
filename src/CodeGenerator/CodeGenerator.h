//
// Created by Neko Toxin on 2022/11/23.
//

#ifndef COMPILER_CODEGENERATOR_H
#define COMPILER_CODEGENERATOR_H

#include <llvm/IR/IRBuilder.h>

using namespace llvm;

class CodeGenerator {
public:
    IRBuilder<> builder;
    llvm::Value *lastValue;
};


#endif //COMPILER_CODEGENERATOR_H
