int main() {
    // 文法不够完善
    // 连加连减可以 连除连乘不可以
    // 还没有比较符号和
    // 文法还应该加入浮点数支持
    int a=1+2+3;
    int b=(a+a)/(a*a);
    while(a){
        a=a-1;
    }

    if (a){
        a=b;
    }else if(b) {
        b = a;
    }else if(a+b){
        // 测试空块
    }else{
        // 函数可以在表达式中调用，但是不能单独调用
        // 应该完善
        a=a+test(a,b,a*b+c);
    }
    return 0;
}