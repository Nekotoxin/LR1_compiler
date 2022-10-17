//
// Created by Motion115 on 2022/10/17
//

#ifndef LEX_TOKEN_H
#define LEX_TOKEN_H

#include <string>
#include <vector>
#include <iostream>
#include <map>
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

	Token(const string& name, const string& token_name, int row, int col) : name(name), token_name(token_name), row(row), col(col){}
	
};

void installToken(const Token& tok);
void printTokenList();
extern vector<Token> token_list;
#endif