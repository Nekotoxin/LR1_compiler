//
// Created by Neko Toxin on 2022/10/14.
//

#include "PreProcess.h"
#include "utils.h"
#include <fstream>
#include <iostream>

const std::string ANY_SYMBOL = "ANY_SYM";
const std::string END_SYMBOL = "$";
const std::string NULL_SYMBOL = "NULL";

PreProcess::PreProcess(std::string source_code_from_lexer_file, std::string grammar_file) {
    auto *grammar = new std::fstream(grammar_file);
    if (!grammar->is_open()) {
        std::cout << "Grammar file not found" << std::endl;
        exit(1);
    } else {
        std::cout << "Grammar file found" << std::endl;
    }

    // get a line first
    std::string line;
    while (std::getline(*grammar, line)) {
        std::string l_val = trim(line);
        if (l_val.empty()) continue;
        std::vector<std::vector<std::string>> r_vals;
        while (std::getline(*grammar, line)) {
            line = trim(line);
            if (line.empty()) continue;
            if (line[0] == ';') break;
            std::vector<std::string> r_val;
            if (line[0] == ':' || line[0] == '|') {
                line = line.substr(1);
                r_val = split(line, ' ');
                r_vals.push_back(r_val);
            }
        }
        for (auto r_val: r_vals) {
            Generator generator(l_val, r_val);
            generators.push_back(generator);
        }
        non_terminals.insert(l_val);
    }

    // add terminals from generators
    for (auto &generator: generators) {
        // insert terminals: not in non_terminals
        for (auto &r_val: generator.second) {
            if (non_terminals.find(r_val) == non_terminals.end()) {
                terminals.insert(r_val);
            }
        }
    }
    // if "NULL" in terminals, remove it
    if (terminals.find(NULL_SYMBOL) != terminals.end()) {
        terminals.erase(NULL_SYMBOL);
    }

    std::cout << "Terminals: " << std::endl;
    for (auto &terminal: terminals) {
        std::cout << terminal << " ";
    }
    std::cout << std::endl;
    std::cout << "Non-terminals: " << std::endl;
    for (auto &non_terminal: non_terminals) {
        std::cout << non_terminal << " ";
    }
    std::cout << std::endl;


}