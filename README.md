# LR1 Compiler

### 1. 使用cmake构建 以及 编译器使用

```sh
git clone https://github.com/Nekotoxin/LR1_compiler
cd LR1_compiler
mkdir build
cd build
cmake ../src
make
./tcc -l ../test_files/regex2token.sl -y ../test_files/grammar.sy -s ../test_files/test.c
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

任务工作量大概 **1+2=3**

1. 错误处理部分, 完全没做, 把我的错误输出删除(那个只说明了,在dfa的当前状态中, 不希望接受此输入, 没有人会愿意看几百个状态). 错误处理参考gcc和yacc, 需要定位到行列

2. AST部分, 在对源代码进行解析的过程中完成AST树的构建. 这部分完全没做

   但是这部分的设计也很重要, 类比TokenStream, 这是语法分析阶段交付给后面步骤的输出

3. identifier继续登陆符号表, 词法分析阶段登陆得还不完全

   ![img](https://img-blog.csdnimg.cn/img_convert/765e08990e400c91e821996783f57ad9.png)

4. 前端部分

   **用户可输入部分**: 词法 .sl 文件, 语法 .sy文件, 源文件 .c文件, 直接给输入框

   设置`初始化lexer`,`初始化parser`,`编译`等按键, **dfa, action和goto表构建相当耗时间, 不要每次都重新使用.sl, .sy文件初始化词法语法分析器**

   

   **前端展示内容**: 可视化DFA[时间不够就不做], 可视化ACTION和GOTO表, 可视化移进规约过程, 可视化语法分析树

   

   1. 前端可以用python写, 用python直接从命令行启动tcc, tcc将json文件保存到本次, 然后python读, 可视化

   2. 前端也可以用js写, 结合qt, qt中嵌入一个浏览器视图用来渲染js文件

      既然有js了,那可视化库就很多了

   3. 如果有别的方法也可以

   https://cyberzhg.github.io/toolbox/lr1, 这是一个开源的文法可视化网页, 除了编译过程都有

   ![Screen Shot 2022-10-21 at 13.32.56](https://user-images.githubusercontent.com/75596353/197121530-3c5a3dfd-ee26-4670-abda-07e6c9a164eb.png)

   ![Screen Shot 2022-10-21 at 13.33.15](https://user-images.githubusercontent.com/75596353/197121548-4d0f2f86-052c-48e5-8cd2-d527f532662b.png)