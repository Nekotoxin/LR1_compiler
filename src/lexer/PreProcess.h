#ifndef LEX_PRE_PROCESS_H
#define LEX_PRE_PROCESS_H

#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include<string>
#include<map>

#define BufferMaxSize 1024
using namespace std;


class PreProcess {
protected:
    ifstream *in;
    map<string, vector<string>> marco;    //宏定义的map
    void dealDefineSentence(const string &str);    //处理宏定义
    vector<string> readWordFromStream(istringstream &str_in);    //从字符串流读取单词
public:
    PreProcess(ifstream *p);    //构造函数，传入文件指针（外部打开关闭）
    bool hasNextLine();

    vector<string> getNextLine();
};

#endif //LEX_PRE_PROCESS_H