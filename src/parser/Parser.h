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
    Parser(){ parsingTable= nullptr;};
    ~Parser();

    AST Parse(TokenStream &token_stream);
    void saveParsingTable(const std::string path1,const std::string path2);  //保存action goto 表
    bool loadParsingTable(const std::string path1,const std::string path2, time_t yacc_file_last_modify_time);  //读取action goto表，0失败，1成功
    AST tree;
};


#endif //PARSER_PARSER_H
