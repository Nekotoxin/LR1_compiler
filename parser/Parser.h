//
// Created by Neko Toxin on 2022/10/14.
//

#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <map>
#include <set>
#include <functional>
#include <string>
#include "PreProcess.h"
#include "ParsingTable.h"


class Parser {
private:
public:
    Parser(std::string source_code_from_lexer_file, std::string grammar_file);
    ~Parser();
    ParsingTable* parsingTable;
    void Parse();
};


#endif //PARSER_PARSER_H
