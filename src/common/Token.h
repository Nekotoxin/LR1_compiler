//
// Created by Motion115 on 2022/10/17
//

#ifndef LEX_TOKEN_H
#define LEX_TOKEN_H

#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <map>
#include <json.hpp>

using namespace std;

class Token {
public:
    string name;
    string token_name;
    int row;
    int col;

    Token() {
        name = "";
        token_name = "";
        row = 0;
        col = 0;
    }

    Token(string name, const string &token_name, int row, int col) : name(std::move(name)), token_name(token_name),
                                                                     row(row), col(col) {}

};


typedef vector<Token> TokenStream;

void installToken(TokenStream &token_stream, const Token &tok);

void printTokenList(const TokenStream &token_stream);

nlohmann::ordered_json TokenListToJson(const TokenStream &token_stream);


#endif