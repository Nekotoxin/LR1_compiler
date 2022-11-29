//int a;
//int b;

int program(int a, int b, int c) {
    int j = a+b;
    if (a > 100) {
        j = a * 2;
    } else {
        j = a;
    }

    if(a> 200) {
        j = a * 3;
    }

    while (j < 1000000) {
        j = j + 1000;
    }
    return j;
}

int demo(int a) {
    a = a + 2;
    return a * 2;
}
