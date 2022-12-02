#include "Lex.h"
#include <regex>
#include <iostream>
#include <stack>
#include <utility>
#include "../common/Token.h"

Lex::Lex(const std::string &regex2token_filepath) {
    lex_file = new ifstream(regex2token_filepath);
    if (!lex_file->is_open()) {
        std::cerr << "regex2token file open failed" << std::endl;
        exit(1);
    }
    std::string token_line;
    //format: "regex" token
    while (std::getline(*lex_file, token_line)) {
        // skip the comment begin with '#'
        if (token_line[0] == '#') continue;
        std::string regex, token;
        // regex between first " and second "
        std::regex reg_regex("\"(.*)\"");
        std::smatch res;
        std::regex_search(token_line, res, reg_regex);
        regex = res[1];
        auto line_res = res.suffix().str();
        // token is the last word in the line
        std::regex reg_token("[^ \t]+$");
        std::regex_search(line_res, res, reg_token);
        token = res[0];
        // 去掉末尾的换行 \n or \r\n
        if (token[token.size() - 1] == '\n') token = token.substr(0, token.size() - 1);
        if (token[token.size() - 1] == '\r') token = token.substr(0, token.size() - 1);
        regex2token.emplace_back(regex, token);
    }


}

Lex::~Lex() {
    delete lex_file;
    lex_file = nullptr;
//    delete source_file;
//    delete pre_process;
}

TokenStream Lex::lexing(const std::string &source_filepath) {
    source_file = new ifstream(source_filepath);
    if (!source_file->is_open()) {
        std::cerr << "source file open failed" << std::endl;
        exit(1);
    }
    pre_process = new PreProcess(source_file);
    TokenStream token_stream;
    // scope stack
    int scope_count = 0;
    int row_no = 0;
    while (pre_process->hasNextLine()) {
        auto line = pre_process->getNextLine();
        row_no++;
        int col_no = 0;
        for (auto &word: line) {
            col_no++;
            // search cache first
            bool mismatch = true;
            // pair result
            std::pair<std::string, std::string> match_res;
            if (match_cache.find(word) != match_cache.end()) {
                mismatch = false;
                match_res = std::make_pair(word, match_cache[word]);
            } else {
                // search regex2token
                for (auto &regex_token: regex2token) {
                    std::regex reg(regex_token.first);
                    std::smatch res;
                    // match the word
                    if (std::regex_match(word, res, reg)) {
                        mismatch = false;
                        match_cache[word] = regex_token.second;
                        match_res = std::make_pair(word, regex_token.second);
                        break;
                    }
                }
            }
            if (mismatch) {
                std::cerr << "mismatch word: \"" << word << "\" at row:" << row_no << " col:" << col_no;
                for (auto &regex_token: regex2token) {
                    std::regex reg(regex_token.first);
                    if (std::regex_search(word, reg)) {
                        std::cerr << ", expected:" << regex_token.second << std::endl;
                        return token_stream;
                    }
                }
                std::cerr << std::endl;
            } else {
                // install to token table
                Token tok;
                tok.name = match_res.first;
                tok.token_name = match_res.second;
                tok.row = row_no;
                tok.col = col_no;
                installToken(token_stream, tok);
            }
        }
        // std::cout << std::endl;
    }
    //std::cout << std::endl;
    //printSymbolTable();
    // std::cout << std::endl;
    printTokenList(token_stream);
    delete pre_process;
    delete source_file;
    pre_process = nullptr;
    source_file = nullptr;
    return token_stream;
}
