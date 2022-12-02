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
    if(a==0){
        return 9999;
    } else{
        a = a + 2;
        return a * 2;
    }
    return 0;
}

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