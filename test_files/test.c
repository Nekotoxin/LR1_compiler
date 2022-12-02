int a;
int b;

int program(int a, int b, int c) {
    int i;
    int j;
    i = 0;
    if (a > (b + c)) {
        j = a + (b * c + 1);
    } else {
        j = a;
    }
    while (j <= 100) {
        j = j * 2;
    }
    return j;
}

int demo(int a) {
    b = 9999;
    int c = 1;
    if (a == 0) {
        int k = -1;
        return b * c * k;
    } else {
        int k = -1;
        a = (a + 2) * k * k;
    }
    return a * 2;
}

//int undefined_var_test() {
//    return ttt;
//}
//
//void undefined_func_test() {
//    func(1);
//    return;
//}
//
//void redefined_var_test() {
//    int a = 1;
//    int a = 2;
//    return;
//}
//
//void redefined_func_test() {
//    return;
//}
//
//void redefined_func_test() {
//    return;
//}
//
//void arg_not_match_test() {
//    program(1, 2);
//}
//
//void arg_type_not_match_test() {
//    program(1.1, 2,3);
//}


//void main() {
//    int a;
//    float b;
//    int c;
//    a = 3;
//    b = 4;
//    c = 2;
//    a = program(a, b, demo(c));
//    return;
//}