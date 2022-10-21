//
// Created by Neko Toxin on 2022/10/15.
//

#ifndef PARSER_PARSINGTABLE_H
#define PARSER_PARSINGTABLE_H

#include <map>
#include <set>
#include <functional>
#include <iostream>
#include <string>
#include "GrammarPreProcess.h"
#include "LR1DFA.h"

typedef enum {
    accept = 'a', shift = 's', reduce = 'r', go = 'g', error = 'e'
} Action;

typedef std::map<std::pair<StateNo, std::string>, std::pair<Action, StateNo>> ParsingTable_;

class ParsingTable {
public:
// 生成式，终结符和非终结符
    const std::vector<Generator> &generators;
    const std::set<std::string> &terminals;
    const std::set<std::string> &non_terminals;
// LR1DFA
    LR1DFA *lr1_dfa;
// LR(1)分析表
    ParsingTable_ action_table;
    ParsingTable_ goto_table;
public:
    ParsingTable(const std::vector<Generator> &generators, const std::set<std::string> &terminals,
                 const std::set<std::string> &non_terminals);

    ~ParsingTable();

    void ConstructParsingTable();

};


#endif //PARSER_PARSINGTABLE_H
