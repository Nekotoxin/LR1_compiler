//
// Created by Neko Toxin on 2022/11/23.
//

#include "CodeGenerator.h"

CodeGenerator::CodeGenerator(SyntaxTree *ast_) {
    ast = ast_;
//    builder = new IRBuilder<>(TheContext);
//    TheModule = std::make_unique<Module>("", TheContext);
}

//Value *CodeGenerator::CodeGenHelper(SyntaxTree *node, std::string node_type) {
//
//}

std::string CodeGenerator::CodeGen() {
    auto context = std::make_unique<LLVMContext>();
    IRBuilder<> builder(*context);

    auto module = std::make_unique<Module>("hello", *context);

    // build a 'main' function
    auto i32 = builder.getInt32Ty();
    auto prototype = FunctionType::get(i32, false);
    Function *main_fn = Function::Create(prototype, Function::ExternalLinkage, "main", module.get());
    BasicBlock *body = BasicBlock::Create(*context, "body", main_fn);
    builder.SetInsertPoint(body);

    // use libc's printf function
    auto i8p = builder.getInt8PtrTy();
    auto printf_prototype = FunctionType::get(i8p, true);
    auto printf_fn = Function::Create(printf_prototype, Function::ExternalLinkage, "printf", module.get());

    // call printf with our string
    auto format_str = builder.CreateGlobalStringPtr("hello world\n");
    builder.CreateCall(printf_fn, {format_str});

    // return 0 from main
    auto ret = ConstantInt::get(i32, 0);
    builder.CreateRet(ret);

    // if you want to print the LLVM IR:
    module->print(llvm::outs(), nullptr);
    return "";
}