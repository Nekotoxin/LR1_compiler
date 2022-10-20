#ifndef LEX_LEX_H
#define LEX_LEX_H

#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <utility>
#include <string>
#include <functional>
#include "global_def.h"
#include "pre_process.h"

class Lex {
private:
    std::vector<std::pair<std::string, std::string>> regex2token;   // From regex2token.l
    std::unordered_map<std::string, std::string> match_cache;
    ifstream *source_file;
    ifstream *lex_file;
    PreProcess *pre_process;

public:
    Lex(const std::string &regex2token_file_path, const std::string &source_filepath);

    ~Lex();

    // from source file to token stream
    void lexing();

};


#endif //LEX_LEX_H
