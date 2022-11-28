//int a;
//int b;

int program(int a, int b, int c) {
    int i;
    int j;
    j = 0;
    i = 0;
    if (a > 100) {
        j = a + (b * c + 1);
    } else {
        j = a;
    }
    while (j < 100) {
        j = j * 2;
    }
    return j;
}

int demo(int a) {
    a = a + 2;
    return a * 2;
}

int main() {
    int a;
    a = 1;
    float b;
    int c;
    b = 4;
    c = 2;
    a = b * c;
    a = program(a, b, demo(c));
    return 0;
}