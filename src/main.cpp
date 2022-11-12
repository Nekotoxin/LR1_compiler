//
// Created by Neko Toxin on 2022/10/21.
//

#include <iostream>
#include "parser/Parser.h"
#include "lexer/Lex.h"


std::string func(std::string lex_file,std::string yacc_file,std::string source_file)
{
    ordered_json  res;
    stringstream str_out;
    stringstream str_err;
    std::streambuf  *old_out=std::cout.rdbuf(str_out.rdbuf());  //重定向到字符串
    std::streambuf  *old_err=std::cerr.rdbuf(str_err.rdbuf());
    Lex l(lex_file, source_file);
    auto token_stream = l.lexing();
    if(!str_err.str().empty()){
        res["state"]="lex failed !";
        res["info"]=str_err.str();
        return res.dump(2);
    }
    res["token_list"]=str_out.str();
    int index=str_out.str().size();
    Parser p;
    AST ast_tree;
    auto flag=p.loadParsingTable("../parse_table.txt","../parse_table.csv");
    if(flag)
         ast_tree=p.Parse(token_stream);
    else{
        Parser pp(yacc_file);
        ast_tree=pp.Parse(token_stream);
    }
    ast_tree.to_json("../json_tree.json");
    if(!str_err.str().empty()){
        res["state"]="parse failed !";
        res["info"]=str_err.str();
        return res.dump(2);
    }
    res["state"]="success";
    res["procedure"]=str_out.str().substr(index);
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

   // func(lex_file,yacc_file,source_file);
    Lex l(lex_file, source_file);
    auto token_stream = l.lexing();

    Parser p;
    auto flag=p.loadParsingTable("../parse_table.txt","../parse_table.csv");
    if(flag)
        auto ast_tree=p.Parse(token_stream);
    else{
        Parser pp(yacc_file);
        auto ast_tree=pp.Parse(token_stream);
    }
    return 0;
}

