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
        auto var_type = DeclStmt->child[0]->child[0]->token_name;
        GlobalVariable *gvar = nullptr;
        if (var_type == "INT") {
            Constant *con_0 = ConstantInt::get(the_context, APInt(32, 0, true));
            gvar = new GlobalVariable(
                    *the_module,
                    Type::getInt32Ty(the_context),
                    false,
                    GlobalValue::CommonLinkage,
                    con_0,
                    DeclStmt->child[1]->name
            );
        } else if (var_type == "FLOAT") {
            Constant *con_0 = ConstantFP::get(the_context, APFloat(0.0));
            gvar = new GlobalVariable(
                    *the_module,
                    Type::getDoubleTy(the_context),
                    false,
                    GlobalValue::CommonLinkage,
                    con_0,
                    DeclStmt->child[1]->name
            );
        } else {
            logError("Unknown type in global def");
        }

        symbol_table.addSymbol(DeclStmt->child[1]->name, gvar);
    }

    // generate all functions
    for (auto FuncDecl: func_decls) {
        CodeGenFunc(FuncDecl);
    }

    if (!errors.empty()) {
        std::cerr << "------------- Error --------------" << std::endl;
        for (auto error: errors) {
            std::cerr << error << std::endl;
        }
        std::cerr << "----------------------------------" << std::endl;
        exit(-1);
    }

    std::string output;
    raw_string_ostream stream(output);
    the_module->print(stream, nullptr);
    return stream.str();
}

//AllocaInst *CodeGenerator::CreateBlockAlloca(Function *func, const std::string &arg_name) {
//    IRBuilder<> tmp_builder(&func->getEntryBlock(), func->getEntryBlock().begin());
//    auto *alloca = tmp_builder.CreateAlloca(Type::getInt32Ty(the_context), nullptr, arg_name);
//    return alloca;
//
//}

Function *CodeGenerator::CodeGenFunc(ASTNode *node) {
    symbol_table.beginScope();

    // func_decl -> type IDENTIFIER ( func_parameter_list ) { stmts }
    auto ret_type = node->child[0]->child[0]->token_name;
    auto func_name = node->child[1]->name;
    auto func_param_list = node->child[3];
    auto stmts = node->child[6];
//    std::vector<std::string> params_def;
    std::vector<std::pair<std::string, std::string>> params_def;
    auto cur_node = func_param_list;

    // check param
    auto is_param_type_error = [](std::string type_name) -> bool {
        return type_name != "INT" && type_name != "FLOAT";
    };

    while (cur_node->name != "NULL") {
        if (cur_node->child.size() == 2) {
            auto param_type = cur_node->child[0]->child[0]->token_name;
            auto param_name = cur_node->child[1]->name;
            if (is_param_type_error(param_type)) {
                auto err_node = cur_node->child[0]->child[0];
                logError(err_node->row, err_node->col, "un-support type '" + param_type + "'");
            }
            params_def.push_back(std::make_pair(param_type, param_name));
            break;
        } else if (cur_node->child.size() == 4) {
            auto param_type = cur_node->child[2]->child[0]->token_name;
            auto param_name = cur_node->child[3]->name;
            if (is_param_type_error(param_type)) {
                auto err_node = cur_node->child[2]->child[0];
                logError(err_node->row, err_node->col, "un-support type '" + param_type + "'");
            }
            params_def.push_back(std::make_pair(param_type, param_name));
            cur_node = cur_node->child[0];
            continue;
        }
        break;
    }

    std::reverse(params_def.begin(), params_def.end());

    std::vector<Type *> args(params_def.size());
    for (int i = 0; i < params_def.size(); i++) {
        if (params_def[i].first == "INT") {
            args[i] = Type::getInt32Ty(the_context);
        } else if (params_def[i].first == "FLOAT") {
            args[i] = Type::getFloatTy(the_context);
        }
    }
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
        auto err_node = node->child[1];
        logError(err_node->row, err_node->col, "redefinition of '" + func_name + "'");
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

    // set function parameters
    for (auto &arg: func->args()) {
        auto arg_def = params_def[arg.getArgNo()];
        arg.setName(arg_def.second);
        IRBuilder<> tmp_builder(&func->getEntryBlock(), func->getEntryBlock().begin());
        AllocaInst *alloca;
        if (arg_def.first == "INT") {
            alloca = tmp_builder.CreateAlloca(Type::getInt32Ty(the_context), nullptr, arg_def.second);
        } else if (arg_def.first == "FLOAT") {
            alloca = tmp_builder.CreateAlloca(Type::getFloatTy(the_context), nullptr, arg_def.second);
        } else {
            // remove func
            func->eraseFromParent();
            return nullptr;
        }
        tmp_builder.CreateStore(&arg, alloca);
        symbol_table.addSymbol(arg.getName().str(), alloca);
    }

    CodeGenHelper(stmts);

    auto ret_code = verifyFunction(*func, &errs());
    if (ret_code) {
//        func->print(errs());
//        logError("LLVM Error: verifyFunction failed");
        return nullptr;
    }

    symbol_table.endScope();
    return nullptr;
}

static std::string GetNodeName(ASTNode *node) {
    return node->type == leafNode ? node->token_name : node->name;
}

Type *CodeGenerator::getLLVMType(const std::string &type_name) {
    if (type_name == "INT") {
        return Type::getInt32Ty(the_context);
    } else if (type_name == "FLOAT") {
        return Type::getFloatTy(the_context);
    } else if (type_name == "VOID") {
        return Type::getVoidTy(the_context);
    } else {
        return nullptr;
    }
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
            auto *ret = CodeGenHelper(stmt->child[0]);
            if (ret) {
                return ret;
            }
        }
        return nullptr;
    }

    if (node_name == "decl_stmt") {
        if (symbol_table.isConflict(node->child[1]->name)) {
            auto err_node = node->child[1];
            logError(err_node->row, err_node->col, "variable " + err_node->name + " already defined");
            return nullptr;
        }
        if (builder->GetInsertBlock()) {
            auto var_type_name = node->child[0]->child[0]->token_name;
            AllocaInst *alloca = builder->CreateAlloca(getLLVMType(var_type_name), nullptr, node->child[1]->name);
            symbol_table.addSymbol(node->child[1]->name, alloca);
            //            named_values[node->child[1]->name] = var;
            if (node->child.size() == 5 && node->child[3]->name == "binop_expr") {
                auto *val = CodeGenHelper(node->child[3]);
                builder->CreateStore(val, alloca);
            }
            return nullptr;
        }
    } else if (node_name == "assign_stmt") {
        // a = 3
        auto *var = symbol_table.getSymbol(node->child[0]->name);
//        auto *var = named_values[node->child[0]->name];
        auto *val = CodeGenHelper(node->child[2]);
        builder->CreateStore(val, var);
        return nullptr;
    } else if (node_name == "if_stmt") {
        //"/Users/nekotoxin/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/222.4167.35/CLion.app/Contents/bin/cmake/mac/bin/cmake" --build /Users/nekotoxin/workspace/gitclone/LR1_compiler/cmake-build-debug --target tcc -j 6 &&  /Users/nekotoxin/workspace/gitclone/LR1_compiler/cmake-build-debug/tcc -l ../test_files/regex2token.sl -y ../test_files/grammar.sy -s ../test_files/test.c && llc code.ll -filetype=obj -o test.o && gcc main.cpp test.o -o test && ./test
        if (node->child.size() == 7) {
            // if_stmt -> IF ( expr ) { stmts }
            auto *cond = CodeGenHelper(node->child[2]);
            auto *then_block = BasicBlock::Create(the_context, "then", builder->GetInsertBlock()->getParent());
            auto *merge_block = BasicBlock::Create(the_context, "merge");
            builder->CreateCondBr(cond, then_block, merge_block);
            builder->SetInsertPoint(then_block);
            symbol_table.beginScope();
            auto ret = CodeGenHelper(node->child[5]);
            symbol_table.endScope();
            if (!ret) {
                builder->CreateBr(merge_block);
            }
            builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(merge_block);
            builder->SetInsertPoint(merge_block);
            return nullptr;
        } else if (node->child.size() == 11) {
            // if_stmt -> IF ( expr ) { stmts } ELSE { stmts }
            auto *cond = CodeGenHelper(node->child[2]);
            auto *then_block = BasicBlock::Create(the_context, "then", builder->GetInsertBlock()->getParent());
            auto *else_block = BasicBlock::Create(the_context, "else");
            auto *merge_block = BasicBlock::Create(the_context, "merge");
            builder->CreateCondBr(cond, then_block, else_block);
            builder->SetInsertPoint(then_block);
            symbol_table.beginScope();
            auto ret = CodeGenHelper(node->child[5]);
            symbol_table.endScope();
            if (!ret) {
                builder->CreateBr(merge_block);
            }
            builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(else_block);
            builder->SetInsertPoint(else_block);
            symbol_table.beginScope();
            ret = CodeGenHelper(node->child[9]);
            symbol_table.endScope();
            if (!ret) {
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
        symbol_table.beginScope();
        auto ret = CodeGenHelper(node->child[5]);
        symbol_table.endScope();
        if (!ret) {
            cond = CodeGenHelper(node->child[2]);
            builder->CreateCondBr(cond, loop_block, merge_block);
        }
        func->getBasicBlockList().push_back(merge_block);
        builder->SetInsertPoint(merge_block);
        return nullptr;
    } else if (node_name == "return_stmt") {
        // return_stmt -> RETURN expr ;
        // return_stmt -> RETURN ;
        auto *func = builder->GetInsertBlock()->getParent();
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
            } else if (op == "EQ_OP") {
                return builder->CreateICmpEQ(lhs, rhs);
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
            // convert lhs to double
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
        } else if (node->child.size() == 3) {
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
        if (node->child.size() == 2) {
            // factor -> - unary_expr
            auto *rhs = CodeGenHelper(node->child[1]);
            return builder->CreateNeg(rhs, "neg_tmp");
        }
        return CodeGenHelper(node->child[0]);
    } else if (node_name == "unary_expr") {
        // unary_expr -> IDENTIFIER || unary_expr -> INT_CONST || unary_expr -> FLOAT_CONST
        if (node->child.size() == 1) {
            if (node->child[0]->token_name == "IDENTIFIER") {
                auto *var = CodeGenHelper(node->child[0]);
                if(var==nullptr){
                    return nullptr;
                }
                Type *var_type;
                if (symbol_table.isGlobalVar(node->child[0]->name)) {
                    var_type = ((GlobalVariable *) var)->getValueType();
                } else {
                    var_type = ((AllocaInst *) var)->getAllocatedType();
                }
                if (var_type->isIntegerTy()) {
                    return builder->CreateLoad(llvm::Type::getInt32Ty(the_context), CodeGenHelper(node->child[0]));
                } else if (var_type->isFloatTy()) {
                    return builder->CreateLoad(llvm::Type::getFloatTy(the_context), CodeGenHelper(node->child[0]));
                }
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
                    auto err_node = node->child[0];
                    logError(err_node->row, err_node->col, "function " + func_name + " not defined");
                    return nullptr;
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
                // check call args type
                auto func_type = func->getFunctionType();
                if (func_type->getNumParams() != args.size()) {
                    auto err_node = node->child[0];
                    logError(err_node->row, "function " + func_name + " call args num not match");
                    return nullptr;
                }
                for (int i = 0; i < args.size(); ++i) {
                    if (args[i]->getType() != func_type->getParamType(i)) {
                        auto err_node = node->child[0];
                        logError(err_node->row, "function " + func_name + " call args type not match");
                        return nullptr;
                    }
                }
                return builder->CreateCall(func, args);
            }
        }
    } else if (node_name == "I_CONSTANT") {
        return ConstantInt::get(the_context, APInt(32, std::stoi(node->name), true));
    } else if (node_name == "F_CONSTANT") {
        return ConstantFP::get(the_context, APFloat(std::stof(node->name)));
    } else if (node_name == "IDENTIFIER") {
        auto *var = symbol_table.getSymbol(node->name);
        if (var == nullptr) {
            logError(node->row, node->col, "variable " + node->name + " not defined");
            return nullptr;
        }
        return var;
    }

    return nullptr;

}

void CodeGenerator::logError(const int line, const int col, const std::string &prompt) {
    errors.push_back("Error: line " + std::to_string(line) + " col " + std::to_string(col) + ": " + prompt);
}

void CodeGenerator::logError(const int line, const std::string &prompt) {
    errors.push_back("Error: line " + std::to_string(line) + ": " + prompt);
}

void CodeGenerator::logError(const std::string &prompt) {
    errors.push_back("Error: " + prompt);
}