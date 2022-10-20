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
    map<string, vector<string>> marco;    //�궨���map
    void dealDefineSentence(const string &str);    //����궨��
    vector<string> readWordFromStream(istringstream &str_in);    //���ַ�������ȡ����
public:
    PreProcess(ifstream *p);    //���캯���������ļ�ָ�루�ⲿ�򿪹رգ�
    bool hasNextLine();

    vector<string> getNextLine();
};

#endif //LEX_PRE_PROCESS_H