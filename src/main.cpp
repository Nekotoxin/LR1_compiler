//
// Created by Neko Toxin on 2022/10/21.
//

#include <iostream>
#include "parser/Parser.h"
#include "lexer/Lex.h"

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

    Lex l(lex_file, source_file);
    auto token_stream = l.lexing();

    Parser p(yacc_file);
    auto ast_tree = p.Parse(token_stream);

    return 0;
}

