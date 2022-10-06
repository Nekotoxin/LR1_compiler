#include "lex.h"

int main() {
    Lex l("../regex2token.l", "../test.c");
    l.lexing();
    return 0;
}
