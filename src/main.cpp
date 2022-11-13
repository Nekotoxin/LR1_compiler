//
// Created by Neko Toxin on 2022/10/21.
//

#include <iostream>
#include "parser/Parser.h"
#include "lexer/Lex.h"
#include <sys/stat.h>

std::string compile(std::string lex_file, std::string yacc_file, std::string source_file) {
    ordered_json res;
    stringstream str_out;
    stringstream str_err;
    std::streambuf *old_out = std::cout.rdbuf(str_out.rdbuf());  //重定向到字符串
    std::streambuf *old_err = std::cerr.rdbuf(str_err.rdbuf());
    auto file_exist = [](const std::string &name) {
        return (access(name.c_str(), F_OK) != -1);
    };

    if (!file_exist(lex_file)) {
        res["state"] = "fail";
        res["error"] = "lex file not exist";
        std::cout.rdbuf(old_out);   //恢复
        std::cerr.rdbuf(old_err);
        return res.dump();
    } else if (!file_exist(yacc_file)) {
        res["state"] = "fail";
        res["error"] = "yacc file not exist";
        std::cout.rdbuf(old_out);   //恢复
        std::cerr.rdbuf(old_err);
        return res.dump();
    } else if (!file_exist(source_file)) {
        res["state"] = "fail";
        res["error"] = "source file not exist";
        std::cout.rdbuf(old_out);   //恢复
        std::cerr.rdbuf(old_err);
        return res.dump();
    }


    Lex l(lex_file);
    auto token_stream = l.lexing(source_file);
    if (!str_err.str().empty()) {
        res["state"] = "fail";
        res["error"] = "lex failed :" + str_err.str();
        std::cout.rdbuf(old_out);   //恢复
        std::cerr.rdbuf(old_err);
        return res.dump(2);
    }
    res["token_list"] = str_out.str();
    int index = str_out.str().size();
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
        res["state"] = "fail";
        res["error"] = "parse failed :" + str_err.str();
        std::cout.rdbuf(old_out);   //恢复
        std::cerr.rdbuf(old_err);
        return res.dump(2);
    }
    auto ast_tree_json = ast_tree.to_json();
    res["state"] = "success";
    res["procedure"] = str_out.str().substr(index);
    res["ast_tree"] = ast_tree_json;
    std::cout.rdbuf(old_out);   //恢复
    std::cerr.rdbuf(old_err);
    return res.dump(2);
}


int main(int argc, char *argv[]) {
    // tcc -l ../test_files/grammar.txt -y ../test_files/grammar.txt -s ../test_files/test.c
    if (argc != 7) {
        std::cout << "Usage: tcc -l <lex_file> -y <grammar_file> -s <source_file>" << std::endl;
        return 0;
    }
    std::string lex_file = "";
    std::string yacc_file = "";
    std::string source_file = "";
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-l") {
            lex_file = argv[i + 1];
        } else if (std::string(argv[i]) == "-y") {
            yacc_file = argv[i + 1];
        } else if (std::string(argv[i]) == "-s") {
            source_file = argv[i + 1];
        }
    }

    auto res = compile(lex_file, yacc_file, source_file);


    std::ofstream out("./res.json");
    out << res;
    out.close();
    return 0;
}

