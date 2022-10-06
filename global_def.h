//
// Created by Neko Toxin on 2022/10/6.
//

#ifndef LEX_GLOBAL_DEF_H
#define LEX_GLOBAL_DEF_H

enum ID_CLASS {
    LOCAL, GLOBAL
};
enum ID_TYPE{
    CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, VOID, STRUCT, POINTER
};

#define CHAR_SIZE 1
#define SHORT_SIZE 2
#define INT_SIZE 4
#define LONG_SIZE 8
#define FLOAT_SIZE 4
#define DOUBLE_SIZE 8
#define POINTER_SIZE 8

#endif //LEX_CPP_GLOBAL_DEF_H
