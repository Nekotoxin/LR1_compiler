# CHANGE LOG

## ver.221017
1. regex2token.l文件修改，原来所有的空行增加#标识，修改程序在MSVC编译器下错误运行的情况
2. main函数Lex类传入文件地址变更，使程序在clone后以GitHub目录结构即可使用
3. 增加token类，属性包括name, token_name, row, col；其中，col代表当行元素的第几列
4. 错误处理在词法环节应该已经具备了
