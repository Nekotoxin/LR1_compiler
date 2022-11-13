//
// Created by Neko Toxin on 2022/10/21.
//

#include <iostream>
#include "parser/Parser.h"
#include "lexer/Lex.h"
#include <sys/stat.h>
#include <iomanip>


ordered_json compile(std::string lex_file, std::string yacc_file, std::string source_file) {
    ordered_json res;
    stringstream str_out;
    stringstream str_err;
    std::streambuf *old_out;
    std::streambuf *old_err;


    auto redirect_to_str = [&]() {
        old_out = std::cout.rdbuf(str_out.rdbuf());  //重定向到字符串
        old_err = std::cerr.rdbuf(str_err.rdbuf());
    };

    auto undo_redirect = [&]() {
        std::cout.rdbuf(old_out);  //恢复
        std::cerr.rdbuf(old_err);
    };

    auto error_handler = [&](const std::string &stage, const std::string &msg) {
        res[stage]["state"] = "fail";
        res[stage]["info"] = msg;
        if (stage == "init_stage") {
            res["lex_stage"]["state"] = "fail";
            res["lex_stage"]["info"] = "failed due to previous failure";
            res["parse_stage"]["state"] = "fail";
            res["parse_stage"]["info"] = "failed due to previous failure";
        } else if (stage == "lex_stage") {
            res["parse_stage"]["state"] = "fail";
            res["parse_stage"]["info"] = "failed due to previous failure";
        }
        undo_redirect();
        return res;
    };

    auto file_exist = [](const std::string &name) {
        return (access(name.c_str(), F_OK) != -1);
    };


    redirect_to_str();

    // Init Stage
    if (!file_exist(lex_file)) {
        return error_handler("init_stage", "lex file not exist");
    } else if (!file_exist(yacc_file)) {
        return error_handler("init_stage", "yacc file not exist");
    } else if (!file_exist(source_file)) {
        return error_handler("init_stage", "source file not exist");
    }
    res["init_stage"]["state"] = "success";

    // Lex Stage
    Lex l(lex_file);
    auto token_stream = l.lexing(source_file);
    if (!str_err.str().empty()) {
        return error_handler("lex_stage", str_err.str());
    }
    auto token_list = TokenListToJson(token_stream);
    res["lex_stage"]["state"] = "success";
    res["lex_stage"]["token_list"] = token_list;
    str_out.str("");

    // Parse Stage
    Parser parser;
    AST ast_tree;
    struct stat result;
    stat(yacc_file.c_str(), &result);
    auto flag = parser.loadParsingTable("./parse_table.txt", "./parse_table.csv", result.st_mtime);
    if (flag)
        parser.Parse(token_stream, ast_tree);
    else {
        Parser parser_regen(yacc_file);
        parser_regen.Parse(token_stream, ast_tree);
    }
    if (!str_err.str().empty()) {
        return error_handler("parse_stage", str_err.str());
    }
    auto ast_tree_json = ast_tree.to_json();
    res["parse_stage"]["state"] = "success";
    res["parse_stage"]["procedure"] = str_out.str();
    res["parse_stage"]["ast_tree"] = ast_tree_json;
    str_out.str("");
    undo_redirect();
    return res;
}

void get_path(int argc, char *argv[], std::string &lex_file, std::string &yacc_file, std::string &source_file) {
    if (argc != 7) {
        std::cout << "Usage: tcc -l <lex_file> -y <grammar_file> -s <source_file>" << std::endl;
        exit(0);
    }

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-l") {
            lex_file = argv[i + 1];
        } else if (std::string(argv[i]) == "-y") {
            yacc_file = argv[i + 1];
        } else if (std::string(argv[i]) == "-s") {
            source_file = argv[i + 1];
        }
    }
}

void output_to_terminal(ordered_json res) {
    if (res["init_stage"]["state"] == "fail") {
        std::string info;
        info = res["init_stage"]["info"];
        std::cout << "Init Stage: " << info << std::endl;
        return;
    } else if (res["lex_stage"]["state"] == "fail") {
        std::string info;
        info = res["lex_stage"]["info"];
        std::cout << "Lex Stage Failed: " << info << std::endl;
        return;
    } else if (res["parse_stage"]["state"] == "fail") {
        std::string info;
        info = res["parse_stage"]["info"];
        std::cout << "Parse Stage Failed: " << info << std::endl;
        return;
    }

    std::cout << res["lex_stage"].dump(4) << std::endl << std::endl;

    std::string procedure = res["parse_stage"]["procedure"];
    std::cout << "Procedure:" << std::endl;
    std::cout << procedure << std::endl << std::endl;

    std::cout << "Abstract Syntax Tree:" << std::endl;
    std::cout << res["parse_stage"]["ast_tree"].dump(1) << std::endl << std::endl;

}

void output_to_file(ordered_json res) {
    std::ofstream out("./res.json");
    out << res;
    out.close();
}


int main(int argc, char *argv[]) {
    // tcc -l ../test_files/grammar.txt -y ../test_files/grammar.txt -s ../test_files/test.c
    std::string lex_file = "", yacc_file = "", source_file = "";
    get_path(argc, argv, lex_file, yacc_file, source_file);
    auto res = compile(lex_file, yacc_file, source_file);
    output_to_terminal(res);
    output_to_file(res);
    return 0;
}

