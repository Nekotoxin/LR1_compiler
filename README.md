# LR1 Compiler

### 1. 使用cmake构建 以及 编译器使用

```sh
git clone https://github.com/Nekotoxin/LR1_compiler
cd LR1_compiler
mkdir build
cd build
cmake ..
make
./tcc -l ../test_files/regex2token.sl -y ../test_files/grammar.sy -s ../test_files/test.c
```
运行中间代码流程：
先使用tcc生成中间代码， 保存到code.ll
再用llvm套件中的llc将code.ll编译成.o文件
再用gcc将main.cpp编译， 和.o文件链接， 生成可执行文件

一键运行的示例：
我的编译配置："/Users/nekotoxin/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/222.4167.35/CLion.app/Contents/bin/cmake/mac/bin/cmake" --build /Users/nekotoxin/workspace/gitclone/LR1_compiler/cmake-build-debug --target tcc -j 6 &&  /Users/nekotoxin/workspace/gitclone/LR1_compiler/cmake-build-debug/tcc -l ../test_files/regex2token.sl -y ../test_files/grammar.sy -s ../test_files/test.c && llc code.ll -filetype=obj -o test.o && gcc main.cpp test.o -o test && ./test

输出样例：
```asm
@a = common global i32 0
@b = common global i32 0

define i32 @program(i32 %a, i32 %b, i32 %c) {
entry:
  %c3 = alloca i32, align 4
  store i32 %c, ptr %c3, align 4
  %b2 = alloca i32, align 4
  store i32 %b, ptr %b2, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  store i32 0, ptr %i, align 4
  %0 = load i32, ptr %a1, align 4
  %1 = load i32, ptr %b2, align 4
  %2 = load i32, ptr %c3, align 4
  %addtmp = add i32 %1, %2
  %3 = icmp sgt i32 %0, %addtmp
  br i1 %3, label %then, label %else

then:                                             ; preds = %entry
  %4 = load i32, ptr %a1, align 4
  %5 = load i32, ptr %b2, align 4
  %6 = load i32, ptr %c3, align 4
  %mul_tmp = mul i32 %5, %6
  %addtmp4 = add i32 %mul_tmp, 1
  %addtmp5 = add i32 %4, %addtmp4
  store i32 %addtmp5, ptr %j, align 4
  br label %merge

else:                                             ; preds = %entry
  %7 = load i32, ptr %a1, align 4
  store i32 %7, ptr %j, align 4
  br label %merge

merge:                                            ; preds = %else, %then
  %8 = load i32, ptr %j, align 4
  %9 = icmp sle i32 %8, 100
  br i1 %9, label %loop, label %merge7

loop:                                             ; preds = %loop, %merge
  %10 = load i32, ptr %j, align 4
  %mul_tmp6 = mul i32 %10, 2
  store i32 %mul_tmp6, ptr %j, align 4
  %11 = load i32, ptr %j, align 4
  %12 = icmp sle i32 %11, 100
  br i1 %12, label %loop, label %merge7

merge7:                                           ; preds = %loop, %merge
  %13 = load i32, ptr %j, align 4
  ret i32 %13
}

define i32 @demo(i32 %a) {
entry:
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 9999, ptr @b, align 4
  %c = alloca i32, align 4
  store i32 1, ptr %c, align 4
  %0 = load i32, ptr %a1, align 4
  %1 = icmp eq i32 %0, 0
  br i1 %1, label %then, label %else

then:                                             ; preds = %entry
  %k = alloca i32, align 4
  store i32 -1, ptr %k, align 4
  %2 = load i32, ptr @b, align 4
  %3 = load i32, ptr %c, align 4
  %mul_tmp = mul i32 %2, %3
  %4 = load i32, ptr %k, align 4
  %mul_tmp2 = mul i32 %mul_tmp, %4
  ret i32 %mul_tmp2

else:                                             ; preds = %entry
  %k3 = alloca i32, align 4
  store i32 -1, ptr %k3, align 4
  %5 = load i32, ptr %a1, align 4
  %addtmp = add i32 %5, 2
  %6 = load i32, ptr %k3, align 4
  %mul_tmp4 = mul i32 %addtmp, %6
  %7 = load i32, ptr %k3, align 4
  %mul_tmp5 = mul i32 %mul_tmp4, %7
  store i32 %mul_tmp5, ptr %a1, align 4
  br label %merge

merge:                                            ; preds = %else
  %8 = load i32, ptr %a1, align 4
  %mul_tmp6 = mul i32 %8, 2
  ret i32 %mul_tmp6
}

----------compiler & run output---------
should be -9999, out: -9999
should be 6, out: 6
should be 192, out: 192
```


正常输出:

![Screen Shot 2022-10-21 at 13.30.24](https://user-images.githubusercontent.com/75596353/197121504-f0977be8-aed6-49a6-b273-be5d1e7db001.png)

### 2. 结构说明

1. .sl文件用于词法分析器构建
2. .sy文件用于语法分析器构建
3. 解析流程:
    1. 使用词法分析器分析源代码, 输入源代码, 输出TokenStream
    2. 使用语法分析器分析语法, 输入TokenStream, 输出AST语法树
4. src/common用于存放lexer和parser共同需要的部分, 比如符号表

### 3. 待办

任务工作量大概 **1+2+3=4**

1. 错误处理部分, 完全没做, 把我的错误输出删除(那个只说明了,在dfa的当前状态中, 不希望接受此输入, 没有人会愿意看几百个状态). 错误处理参考gcc和yacc, 需要定位到行列

2. 进一步完善文法, 请翻到本文件最后查看希望成功解析的源文件

   还需要做以下支持

    1. 现在只支持一个函数, 将来要支持多个函数
    2. 支持连续比较: <, >, <=等等
    3. 支持连乘连除

3. AST部分, 在对源代码进行解析的过程中完成AST树的构建. 这部分完全没做

   但是这部分的设计也很重要, 类比TokenStream, 这是语法分析阶段交付给后面步骤的输出

4. identifier继续登陆符号表, 词法分析阶段登陆得还不完全

   ![img](https://img-blog.csdnimg.cn/img_convert/765e08990e400c91e821996783f57ad9.png)

5. 前端部分

   **用户可输入部分**: 词法 .sl 文件, 语法 .sy文件, 源文件 .c文件, 直接给输入框

   设置`初始化lexer`,`初始化parser`,`编译`等按键, **dfa, action和goto表构建相当耗时间, 不要每次都重新使用.sl, .sy文件初始化词法语法分析器**

**前端展示内容**: 可视化DFA[时间不够就不做], 可视化ACTION和GOTO表, 可视化移进规约过程, 可视化语法分析树

1. 前端可以用python写, 用python直接从命令行启动tcc, tcc将json文件保存到本地, 然后python读, 可视化

2. 前端也可以用js写, 结合qt, qt中嵌入一个浏览器视图用来渲染js文件

   既然有js了,那可视化库就很多了

3. 如果有别的方法也可以

https://cyberzhg.github.io/toolbox/lr1, 这是一个开源的文法可视化网页, 除了编译过程都有

![Screen Shot 2022-10-21 at 13.32.56](https://user-images.githubusercontent.com/75596353/197121530-3c5a3dfd-ee26-4670-abda-07e6c9a164eb.png)

![Screen Shot 2022-10-21 at 13.33.15](https://user-images.githubusercontent.com/75596353/197121548-4d0f2f86-052c-48e5-8cd2-d527f532662b.png)


```c
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
    a = a + 2;
    return a * 2;
}

void main(void) {
    int a = 1*2/3;
    float b;
    int c;
    a = 3;
    b = 4;
    c = 2;
    a = program(a, b, demo(c));
    return;
}
```

