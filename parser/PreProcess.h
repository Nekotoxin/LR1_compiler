//
// Created by Neko Toxin on 2022/10/14.
//

#ifndef PARSER_PREPROCESS_H
#define PARSER_PREPROCESS_H

#include <vector>
#include <set>
#include <string>

extern const std::string ANY_SYMBOL;
extern const std::string END_SYMBOL;
extern const std::string NULL_SYMBOL;

typedef std::pair<std::string, std::vector<std::string>> Generator;

class PreProcess {
public:
    std::vector<Generator> generators;
    std::set<std::string> terminals;
    std::set<std::string> non_terminals;
    std::string start_symbol;
public:
    PreProcess(std::string source_code_from_lexer_file, std::string grammar_file);
//    std::vector<Generator> getGenerators();
};


#endif //PARSER_PREPROCESS_H
