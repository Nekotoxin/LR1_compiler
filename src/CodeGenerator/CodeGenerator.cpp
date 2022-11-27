//
// Created by Neko Toxin on 2022/11/23.
//

#include "CodeGenerator.h"

CodeGenerator::CodeGenerator(AST *ast_) {
    ast = ast_;
    builder = new IRBuilder<>(the_context);
    the_module = std::make_unique<Module>("", the_context);
}


std::string CodeGenerator::CodeGen() {
    // get all top level functions and declarations
    auto *cur = ast->root;
    std::vector<ASTNode *> var_decls;
    std::vector<ASTNode *> func_decls;
    while (cur->child.size()==2) {
        auto *top_level_node = cur->child[1];
        if (top_level_node->name == "decl_stmt") {
            // var decl
            var_decls.push_back(top_level_node);
        } else if (top_level_node->name == "func_decl") {
            // func_decl
            func_decls.push_back(top_level_node);
        }
        cur = cur->child[0];
    }

    // grammar is left recursive, so we need to reverse the order
    std::reverse(var_decls.begin(), var_decls.end());
    std::reverse(func_decls.begin(), func_decls.end());

    // generate all global  variables declarations
    for (auto DeclStmt: var_decls) {
        Constant *con_0 = ConstantInt::get(the_context, APInt(32, 0, true));
        GlobalVariable *gvar = new GlobalVariable(
                *the_module,
                Type::getInt32Ty(the_context),
                false,
                GlobalValue::CommonLinkage,
                con_0,
                DeclStmt->child[0]->child[0]->token_name
        );
    }

    // generate all functions
    for (auto FuncDecl: func_decls) {
        CodeGenFunc(FuncDecl);
    }
    auto& ret = the_module->getFunctionList();

    return "";
}

Function *CodeGenerator::CodeGenFunc(ASTNode *node) {
    auto named_values_bak = named_values;

    // func_decl -> type IDENTIFIER ( func_parameter_list ) { stmts }
    auto ret_type = node->child[0]->child[0]->token_name;
    auto func_name = node->child[1]->name;
    auto func_param_list = node->child[3];
    auto stmts = node->child[6];

    std::vector<std::string> param_names;
    auto cur_node = func_param_list;

    while (cur_node->name != "NULL") {
        if (cur_node->child.size() == 2) {
            param_names.push_back(cur_node->child[1]->name);
            break;
        } else if (cur_node->child.size() == 4) {
            param_names.push_back(cur_node->child[3]->name);
            cur_node = cur_node->child[0];
            continue;
        }
        break;
    }

    std::reverse(param_names.begin(), param_names.end());

    std::vector<Type *> args(param_names.size(), Type::getInt32Ty(the_context));
    // generate function
    FunctionType *func_type;
    if (ret_type == "VOID") {
        func_type = FunctionType::get(Type::getVoidTy(the_context), args, false);
    } else if (ret_type == "INT") {
        func_type = FunctionType::get(Type::getInt32Ty(the_context), args, false);
    } else if (ret_type == "FLOAT") {
        func_type = FunctionType::get(Type::getFloatTy(the_context), args, false);
    }

    auto* func_def = the_module->getFunction(func_name);
    if (func_def && !func_def->empty()) {
        logError("Function redefinition " + func_name);
        exit(-1);
    }

    Function *func = Function::Create(
            func_type,
            Function::ExternalLinkage,
            func_name,
            the_module.get()
    );


    // set function parameters
    for (auto &arg: func->args()) {
        arg.setName(param_names[arg.getArgNo()]);
        named_values[param_names[arg.getArgNo()]] = &arg;
    }

    verifyFunction(*func);

    auto *basic_block = BasicBlock::Create(the_context, "entry", func);
    builder->SetInsertPoint(basic_block);
    Value *ret_val = nullptr;

    // print code
//    the_module->print(outs(), nullptr);
    if ((ret_val = CodeGenHelper(stmts))) {
        builder->CreateRet(ret_val);
        return func;
    }

    func->eraseFromParent();
    named_values = named_values_bak;
    return nullptr;
}


Value *CodeGenerator::CodeGenHelper(ASTNode *node) {
    if (node->name == "stmts") {
        std::vector<ASTNode *> stmts;
        auto cur_node = node;
        while (cur_node->child.size()==2) {
            stmts.push_back(cur_node->child[1]);
            cur_node = cur_node->child[0];
        }
        std::reverse(stmts.begin(), stmts.end());
        for (auto stmt: stmts) {
            CodeGenHelper(stmt->child[0]);
        }
    } else if (node->name == "decl_stmt") {
        if(named_values.find(node->child[0]->token_name) != named_values.end()) {
            logError("Variable redefinition " + node->child[0]->child[0]->token_name);
            exit(-1);
        }
        if(builder->GetInsertBlock()){
            auto* func = builder->GetInsertBlock()->getParent();
            auto* alloca = builder->CreateAlloca(Type::getInt32Ty(the_context), nullptr, node->child[0]->child[0]->token_name);
            named_values[node->child[0]->child[0]->token_name] = alloca;
            return nullptr;
        }
    } else if (node->name == "assign_stmt") {

    } else if (node->name == "if_stmt") {

    } else if (node->name == "while_stmt") {

    } else if (node->name == "return_stmt") {

    } else if (node->name == "expr_stmt") {

    }

    return nullptr;

}

void CodeGenerator::logError(const std::string &prompt) {
    errors.push_back(prompt);
}