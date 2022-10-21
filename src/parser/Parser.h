//
// Created by Neko Toxin on 2022/10/14.
//

#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <map>
#include <set>
#include <functional>
#include <string>
#include "GrammarPreProcess.h"
#include "ParsingTable.h"
#include "Token.h"
#include "AST.h"


class Parser {
private:
public:
    std::vector<Generator> generators;
    std::set<std::string> terminals;
    std::set<std::string> non_terminals;
    ParsingTable *parsingTable;

    Parser(std::string grammar_file);

    ~Parser();

    AST Parse(TokenStream &token_stream);
};


#endif //PARSER_PARSER_H
