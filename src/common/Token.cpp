#include "Token.h"
#include <iomanip>


TokenStream token_stream;

void installToken(TokenStream &token_stream, const Token &tok) {
    token_stream.push_back(tok);
}

void printTokenList(const TokenStream &token_stream) {
    cout << "Token List" << endl;
    cout << setiosflags(ios::left) << setw(20) << "name"
         << setw(20) << "token_name"
         << setw(10) << "row"
         << setw(10) << "col"
         << std::endl;
    for (auto &item: token_stream) {
        cout << setw(20) << item.name
             << setw(20) << item.token_name
             << setw(10) << item.row
             << setw(10) << item.col
             << endl;
    }
}

nlohmann::ordered_json TokenListToJson(const TokenStream &token_stream) {
    nlohmann::ordered_json j;
    for (auto &item: token_stream) {
        nlohmann::ordered_json item_json;
        item_json["name"] = item.name;
        item_json["token_name"] = item.token_name;
        item_json["row"] = item.row;
        item_json["col"] = item.col;
        j.push_back(item_json);
    }
    return j;
}