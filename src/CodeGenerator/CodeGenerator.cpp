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
    while (cur->child.size() == 2) {
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
        gvar->setName(DeclStmt->child[1]->name);
    }

    // generate all functions
    for (auto FuncDecl: func_decls) {
        CodeGenFunc(FuncDecl);
    }

    std::string output;
    raw_string_ostream stream(output);
    the_module->print(stream, nullptr);
//    the_module->print(outs(), nullptr);
    return stream.str();
}

AllocaInst* CodeGenerator::CreateBlockAlloca(Function* func, const std::string& arg_name) {
    IRBuilder<> tmp_builder(&func->getEntryBlock(), func->getEntryBlock().begin());
    auto *alloca = tmp_builder.CreateAlloca(Type::getInt32Ty(the_context), nullptr, arg_name);
    return alloca;

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

    auto *func_def = the_module->getFunction(func_name);
    if (func_def && !func_def->empty()) {
        logError("Function redefinition " + func_name);
        return nullptr;
    }

    Function *func = Function::Create(
            func_type,
            Function::ExternalLinkage,
            func_name,
            the_module.get()
    );





    auto *basic_block = BasicBlock::Create(the_context, "entry", func);
    builder->SetInsertPoint(basic_block);
    Value *ret_val = nullptr;

    // set function parameters
    for (auto &arg: func->args()) {
        arg.setName(param_names[arg.getArgNo()]);
        IRBuilder<> tmp_builder(&func->getEntryBlock(), func->getEntryBlock().begin());
        auto *alloca = tmp_builder.CreateAlloca(Type::getInt32Ty(the_context), nullptr, param_names[arg.getArgNo()]);
        tmp_builder.CreateStore(&arg, alloca);
        named_values[param_names[arg.getArgNo()]] = alloca;
    }

    CodeGenHelper(stmts);

    auto ret_code = verifyFunction(*func,&errs());
    if (ret_code) {
        func->print(errs());
        exit(-1);
    }

//    func->eraseFromParent();
    named_values = named_values_bak;
    return nullptr;
}

static std::string GetNodeName(ASTNode *node) {
    return node->type == leafNode ? node->token_name : node->name;
}


Value *CodeGenerator::CodeGenHelper(ASTNode *node) {
    std::string node_name = GetNodeName(node);
    if (node_name == "stmts") {
        std::vector<ASTNode *> stmts;
        auto cur_node = node;
        while (cur_node->child.size() == 2) {
            stmts.push_back(cur_node->child[1]);
            cur_node = cur_node->child[0];
        }
        std::reverse(stmts.begin(), stmts.end());
        for (auto stmt: stmts) {
            auto* ret = CodeGenHelper(stmt->child[0]);
            if(ret){
                return ret;
            }
        }
        return nullptr;
    }

    if (node_name == "decl_stmt") {
        if (named_values.find(node->child[1]->name) != named_values.end()) {
            logError("Variable redefinition " + node->child[0]->child[0]->token_name);
            exit(-1);
        }
        if (builder->GetInsertBlock()) {
            auto *var = builder->CreateAlloca(Type::getInt32Ty(the_context));
            named_values[node->child[1]->name] = var;
            if(node->child.size()==5&&node->child[3]->name=="binop_expr"){
                auto* val = CodeGenHelper(node->child[3]);
                builder->CreateStore(val, var);
            }
            return nullptr;
        }
    } else if (node_name == "assign_stmt") {
        // a = 3
        auto *var = named_values[node->child[0]->name];
        auto *val = CodeGenHelper(node->child[2]);
        builder->CreateStore(val, var);
        return nullptr;
    } else if (node_name == "if_stmt") {
        //"/Users/nekotoxin/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/222.4167.35/CLion.app/Contents/bin/cmake/mac/bin/cmake" --build /Users/nekotoxin/workspace/gitclone/LR1_compiler/cmake-build-debug --target tcc -j 6 &&  /Users/nekotoxin/workspace/gitclone/LR1_compiler/cmake-build-debug/tcc -l ../test_files/regex2token.sl -y ../test_files/grammar.sy -s ../test_files/test.c && llc code.ll -filetype=obj -o test.o && gcc main.cpp test.o -o test && ./test
        if(node->child.size()==7) {
            // if_stmt -> IF ( expr ) { stmts }
            auto *cond = CodeGenHelper(node->child[2]);
            auto *then_block = BasicBlock::Create(the_context, "then", builder->GetInsertBlock()->getParent());
            auto *merge_block = BasicBlock::Create(the_context, "merge");
            builder->CreateCondBr(cond, then_block, merge_block);
            builder->SetInsertPoint(then_block);
            auto ret = CodeGenHelper(node->child[5]);
            if(!ret){
                builder->CreateBr(merge_block);
            }
            builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(merge_block);
            builder->SetInsertPoint(merge_block);
            return nullptr;
        } else if(node->child.size()==11){
            // if_stmt -> IF ( expr ) { stmts } ELSE { stmts }
            auto *cond = CodeGenHelper(node->child[2]);
            auto *then_block = BasicBlock::Create(the_context, "then", builder->GetInsertBlock()->getParent());
            auto *else_block = BasicBlock::Create(the_context, "else");
            auto *merge_block = BasicBlock::Create(the_context, "merge");
            builder->CreateCondBr(cond, then_block, else_block);
            builder->SetInsertPoint(then_block);
            auto ret = CodeGenHelper(node->child[5]);
            if(!ret){
                builder->CreateBr(merge_block);
            }
            builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(else_block);
            builder->SetInsertPoint(else_block);
            ret = CodeGenHelper(node->child[9]);
            if(!ret){
                builder->CreateBr(merge_block);
            }
            builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(merge_block);
            builder->SetInsertPoint(merge_block);
            return nullptr;
        }
        return nullptr;
    } else if (node_name == "while_stmt") {
        // while_stmt -> WHILE ( binop_expr ) { stmts }
        auto *cond = CodeGenHelper(node->child[2]);
        auto *func = builder->GetInsertBlock()->getParent();
        auto *loop_block = BasicBlock::Create(the_context, "loop", func);
        auto *merge_block = BasicBlock::Create(the_context, "merge");
        builder->CreateCondBr(cond, loop_block, merge_block);
        builder->SetInsertPoint(loop_block);
        auto ret = CodeGenHelper(node->child[5]);
        if(!ret){
            cond = CodeGenHelper(node->child[2]);
            builder->CreateCondBr(cond, loop_block, merge_block);
        }
        func->getBasicBlockList().push_back(merge_block);
        builder->SetInsertPoint(merge_block);
        return nullptr;
    } else if (node_name == "return_stmt") {
        // return_stmt -> RETURN expr ;
        // return_stmt -> RETURN ;
        auto* func = builder->GetInsertBlock()->getParent();
        if (node->child.size() == 2) {
            return builder->CreateRetVoid();
        } else {
            return builder->CreateRet(CodeGenHelper(node->child[1]));
        }
    } else if (node_name == "expr_stmt") {
        CodeGenHelper(node->child[0]);
        return nullptr;
    }

    if (node_name == "binop_expr") {
        if (node->child.size() == 1) {
            // binop_expr -> add_expr
            return CodeGenHelper(node->child[0]);
        } else if (node->child.size() == 3) {
            // binop_expr -> binop_expr binop add_expr
            auto *lhs = CodeGenHelper(node->child[0]);
            auto *rhs = CodeGenHelper(node->child[2]);
            auto op = node->child[1]->token_name;
            // < > GE_OP LE_OP
            if (op == "<") {
                return builder->CreateICmpSLT(lhs, rhs);
            } else if (op == ">") {
                return builder->CreateICmpSGT(lhs, rhs);
            } else if (op == "GE_OP") {
                return builder->CreateICmpSGE(lhs, rhs);
            } else if (op == "LE_OP") {
                return builder->CreateICmpSLE(lhs, rhs);
            }
        }
    } else if (node_name == "add_expr") {
        if (node->child.size() == 1 && node->child[0]->name == "item") {
            // add_expr -> item
            return CodeGenHelper(node->child[0]);
        } else if (node->child.size() == 3) {
            // add_expr -> add_expr + item
            auto op = node->child[1]->token_name;
            auto *lhs = CodeGenHelper(node->child[0]);
            auto *rhs = CodeGenHelper(node->child[2]);
            if (op == "+") {
                return builder->CreateAdd(lhs, rhs, "addtmp");
            } else if (op == "-") {
                return builder->CreateSub(lhs, rhs, "subtmp");
            }
        }
    } else if (node_name == "item") {
        if (node->child.size() == 1 && node->child[0]->name == "factor") {
            // item -> factor
            return CodeGenHelper(node->child[0]);
        }
        else if (node->child.size() == 3) {
            // item -> item * factor
            auto op = node->child[1]->token_name;
            auto *lhs = CodeGenHelper(node->child[0]);
            auto *rhs = CodeGenHelper(node->child[2]);
            if (op == "*") {
                return builder->CreateMul(lhs, rhs, "mul_tmp");
            } else if (op == "/") {
                return builder->CreateSDiv(lhs, rhs, "div_tmp");
            }
        }
    } else if (node_name == "factor") {
        if(node->child.size()==2){
            // factor -> - unary_expr
            auto *rhs = CodeGenHelper(node->child[1]);
            return builder->CreateNeg(rhs, "neg_tmp");
        }
        return CodeGenHelper(node->child[0]);
    }
    else if (node_name == "unary_expr") {
        // factor -> IDENTIFIER || factor -> INT_CONST || factor -> FLOAT_CONST
        if (node->child.size() == 1) {
            if (node->child[0]->token_name == "IDENTIFIER") {
                return builder->CreateLoad(llvm::Type::getInt32Ty(the_context), CodeGenHelper(node->child[0]));
            }
            return CodeGenHelper(node->child[0]);
        }
            // factor -> ( binop_expr )
        else if (node->child.size() == 3 && GetNodeName(node->child[0]) == "(" &&
                 GetNodeName(node->child[1]) == "binop_expr" && GetNodeName(node->child[2]) == ")") {

            return CodeGenHelper(node->child[1]);
        }
            //factor -> IDENTIFIER ( parameter_list )
        else if (node->child.size() == 4 && GetNodeName(node->child[0]) == "IDENTIFIER" &&
                 GetNodeName(node->child[1]) == "(" && GetNodeName(node->child[2]) == "parameter_list" &&
                 GetNodeName(node->child[3]) == ")") {
            {
                // function call
                auto func_name = node->child[0]->name;
                auto func = the_module->getFunction(func_name);
                if (!func) {
                    logError("Function not found " + func_name);
                    exit(-1);
                }
                std::vector<ASTNode *> param_names;
                auto cur_node = node->child[2];// parameter_list
                // parameter_list -> binop_expr
                // parameter_list -> parameter_list , binop_expr
                while (cur_node->name != "NULL") {
                    if (cur_node->child.size() == 1) {
                        param_names.push_back(cur_node->child[0]);
                        break;
                    } else if (cur_node->child.size() == 3) {
                        param_names.push_back(cur_node->child[2]);
                        cur_node = cur_node->child[0];
                        continue;
                    }
                    break;
                }
                std::reverse(param_names.begin(), param_names.end());
                std::vector<Value *> args;
                for (auto &param_name: param_names) {
                    args.push_back(CodeGenHelper(param_name));
                }
                return builder->CreateCall(func, args);
            }
        }
    } else if (node_name == "I_CONSTANT") {
        return ConstantInt::get(the_context, APInt(32, std::stoi(node->name), true));
    } else if (node_name == "F_CONSTANT") {
        return ConstantFP::get(the_context, APFloat(std::stof(node->name)));
    } else if (node_name == "IDENTIFIER") {
        auto *var = named_values[node->name];
        if (!var) {
            logError("Variable not found " + node->name);
            exit(-1);
        }
//        if (!var) {
//            Value *g_var = the_module->getGlobalVariable(GetNodeName(node));
//            if (!g_var) {
//                logError("Variable not found " + node->name);
//                exit(-1);
//            }
//            return g_var;
//        }
        return var;
    }

    return nullptr;

}

void CodeGenerator::logError(const std::string &prompt) {
    errors.push_back(prompt);
}