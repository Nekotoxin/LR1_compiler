#include "token.h"
#include <iomanip>

vector<Token> token_list;

void installToken(const Token& tok) {
	token_list.push_back(tok);
}

void printTokenList() {
	cout << "Token List" << endl;
    cout << setiosflags(ios::left) << setw(20) << "name"
        << setw(20) << "token_name"
        << setw(10) << "row"
        << setw(10) << "col"
        << std::endl;
	for (auto& item : token_list) {
        cout << setw(20) << item.name
            << setw(20) << item.token_name
            << setw(10) << item.row
            << setw(10) << item.col
            << endl;
	}
}