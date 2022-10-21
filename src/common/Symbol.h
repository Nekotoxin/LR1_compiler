#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <map>

enum ID_CLASS {
    LOCAL, GLOBAL
};
enum ID_TYPE {
    CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, VOID, STRUCT, POINTER
};

#define CHAR_SIZE 1
#define SHORT_SIZE 2
#define INT_SIZE 4
#define LONG_SIZE 8
#define FLOAT_SIZE 4
#define DOUBLE_SIZE 8
#define POINTER_SIZE 8

class Symbol {
public:
    std::string name;   /* name of the symbol */
    unsigned long hash;           /* hash value of the symbol */
    int type;           /* type of the symbol */
    int binding;        /* LOCAL or GLOBAL */
    int section;        /* section number */
    unsigned long value;    /* section offset or address */
    unsigned long size;     /* object size */

    Symbol() {
        name = "";
        hash = 0;
        type = 0;
        binding = 0;
        section = 0;
        value = 0;
        size = 0;
    }

    Symbol(const std::string &name, unsigned long hash, int type, int binding, int section, unsigned long value,
           unsigned long size) : name(name), hash(hash), type(type), binding(binding), section(section), value(value),
                                 size(size) {}
};

extern std::map<unsigned long, Symbol> symbol_table;

void installSymbol(const Symbol &sym);

void printSymbolTable();
