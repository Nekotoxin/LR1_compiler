//
// Created by Neko Toxin on 2022/11/28.
//

#include <stdio.h>
extern "C" int demo(int);
extern "C" int program(int, int, int);
int main() {
    printf("----------output---------\n");
    printf("should be -9999, out: %d\n", demo(0));
    printf("should be 6, out: %d\n", demo(1));
    printf("should be 192, out: %d\n", program(3, 4, demo(2)));
    return 0;
}