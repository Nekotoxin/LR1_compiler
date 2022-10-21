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

class GrammarPreProcess {
public:
    std::vector<Generator> generators;
    std::set<std::string> terminals;
    std::set<std::string> non_terminals;
    std::string start_symbol;
public:
    GrammarPreProcess(std::string grammar_file);

};


#endif //PARSER_PREPROCESS_H
