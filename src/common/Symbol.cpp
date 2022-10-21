//
// Created by Neko Toxin on 2022/10/6.
//

#include <iomanip>
#include "Symbol.h"

std::map<unsigned long, Symbol> symbol_table;


void installSymbol(const Symbol &sym) {
    if (symbol_table.find(sym.hash) == symbol_table.end()) {
        symbol_table[sym.hash] = sym;
    }
}

void printSymbolTable() {
    std::cout << "Symbol Table(Identifier):" << std::endl;
    std::cout << std::setiosflags(std::ios::left) << std::setw(20) << "name"
              << std::setw(10) << "hash"
              << std::setw(10) << "type"
              << std::setw(10) << "binding"
              << std::setw(10) << "section"
              << std::setw(20) << "value"
              << std::setw(20) << "size"
              << std::endl;

    for (auto &item: symbol_table) {
        std::cout << std::setw(20) << item.second.name
                  << std::setw(10) << (item.second.hash) % 10000
                  << std::setw(10) << item.second.type
                  << std::setw(10) << (item.second.binding == ID_CLASS::GLOBAL ? "GLOBAL" : "LOCAL")
                  << std::setw(10) << "NOT_IMP"
                  << std::setw(20) << "NOT_IMP"
                  << std::setw(20) << "NOT_IMP"
                  << std::endl;
    }
}
