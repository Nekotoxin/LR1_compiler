#include "PreProcess.h"
#include<iostream>
#include<cstring>

static void addSpaceToSign(string &str, int index, int sign_width) {
    str.insert(index, 1, ' ');
    str.insert(index + sign_width + 1, 1, ' ');
}

vector<string> PreProcess::readWordFromStream(istringstream &str_in) {
    vector<string> res;
    while (!str_in.eof()) {
        string tmp;
        str_in >> tmp;
        if (tmp != "" && tmp != "'" && tmp != "\"") {
            auto iter = marco.find(tmp);
            if (iter != marco.end()) {
                auto substitute = iter->second;
                for (auto elem: substitute)
                    res.push_back(elem);
            } else
                res.push_back(tmp);
        }
        if (tmp == "\"" || tmp == "'") {
            const char flag = tmp[0];
            string str_to_read;
            int prev = 0, cur = 0;
            cur = str_in.get();
            bool is_found = 0;
            while (!str_in.eof()) {
                cur = str_in.get();
                if (cur == flag && prev != '\\') {
                    is_found = 1;
                    break;
                } else
                    str_to_read.push_back(char(cur));
                prev = cur;
            }
            if (is_found) {
                str_to_read.pop_back();
                str_to_read.insert(0, 1, flag);
                str_to_read.push_back(flag);
                res.push_back(str_to_read);
            }
        }
    }
    return res;
}

void PreProcess::dealDefineSentence(const string &str) {
    istringstream str_in(str);
    string first_key;
    str_in >> first_key >> first_key >> first_key;
    vector<string> second_key_list = readWordFromStream(str_in);
    marco.insert(pair<string, vector<string>>(first_key, second_key_list));
}

static string stdPoundSentence(const string &str) {
    string res = str;
    for (int k = 0; k < res.size(); k++) {
        if (res[k] == '#') {
            addSpaceToSign(res, k, 1);
            k += 2;
        }
    }
    int include_index = 0;
    while ((include_index = res.find("include", include_index)) != string::npos) {
        res.insert(include_index + 7, 1, ' ');
        include_index += 8;
    }

    int define_index = 0;
    while ((define_index = res.find("define", define_index)) != string::npos) {
        res.insert(define_index + 6, 1, ' ');
        define_index += 7;
    }
    return res;
}

PreProcess::PreProcess(ifstream *p) {
    in = p;
}

bool PreProcess::hasNextLine() {
//    auto preIndex = in->tellg();
//    char buffer[BufferMaxSize];
    if (in->eof())
        return false;
    return true;
//    in->getline(buffer, sizeof(buffer));
//    auto len = strlen(buffer);
//    if (!in->eof() || len != 0) {
//        in->seekg(preIndex);
//        return true;
//    } else {
//        in->seekg(preIndex);
//        return false;
//    }
}

vector<string> PreProcess::getNextLine() {
    char buffer[BufferMaxSize];
    // get line \r\n
    char ch;
    int count=0;
    while ((ch = in->get()) != EOF) {
        //save to buffer
        if (ch == '\r') {
            ch = in->get();
            if (ch == '\n') {
                break;
            } else {
                cerr << "encode error" << endl;
                break;
            }
        } else if (ch == '\n') {
            break;
        } else {
            buffer[count++] = ch;
        }
    }
    buffer[count] = '\0';
//    in->getline(buffer, sizeof(buffer));
    string str_buffer = buffer;

    bool is_include = 0;        //???????????????
    bool is_left_trail = 0;        //????????????
    bool is_string = 0;            //????????????
    bool is_char = 0;
    bool is_define = 0;            //????????
    static bool is_multi_comment = 0; /*?????????*/
    bool is_single_comment = 0;    //???е??????
    bool is_pound_key = 0; // #???


    for (int k = 0; k < str_buffer.size(); k++) {
        //??????δ????????????
        if (is_multi_comment) {
            auto index = str_buffer.find("*/");
            if (index != string::npos) {
                is_multi_comment = 0;
                str_buffer.erase(0, index + 2);
            } else {
                str_buffer.erase(0);
                break;
            }
        }
        if (str_buffer[k] == ' ')
            continue;
        else if (str_buffer[k] == '"') {
            if ((k == 0 || (str_buffer[k - 1] != '\\') && !(str_buffer[k - 1] == '\'' && !is_string)) &&
                !(is_single_comment || is_multi_comment)) {
                addSpaceToSign(str_buffer, k, 1);
                k += 2;
                is_string = !is_string;
            }
        } else if (str_buffer[k] == '\'') {
            if ((k == 0 || (str_buffer[k - 1] != '\\' && !is_string)) && !(is_single_comment || is_multi_comment)) {
                addSpaceToSign(str_buffer, k, 1);
                k += 2;
                is_char = !is_char;
            }
        }
        else if ((str_buffer[k] == '/') && (k + 1 < str_buffer.size()) && (str_buffer[k + 1] == '/') &&
                 !(is_string || is_char) && !(is_include && is_left_trail)) {
            str_buffer.erase((k - 1) >= 0 ? k - 1 : 0);
            is_single_comment = 1;
            break;
        }
        else if (str_buffer[k] == '/' && k + 1 < str_buffer.size() && str_buffer[k + 1] == '*' &&
                 !(is_string || is_char) && !(is_include && is_left_trail)) {
            if (!is_multi_comment)    //???????? /*
                is_multi_comment = 1;
            if (k + 2 < str_buffer.size()) {    //?ж????????? */
                string after = str_buffer.substr(k + 2);
                auto index = after.find("*/");
                if (index != string::npos) {    //?? */ ???????
                    is_multi_comment = 0;
                    str_buffer.erase(k, k + 2 + index + 1 - k + 1);
                    k -= 1;
                } else {
                    str_buffer.erase(k);    //?????????в???
                    break;
                }
            } else {
                str_buffer.erase(k);    //?????????в???
                break;
            }

        }
        else if (str_buffer[k] == '#' && !(is_string || is_char)) {
            is_pound_key = 1;    //???λ??1
            auto std_res = stdPoundSentence(str_buffer.substr(k));
            (str_buffer.erase(k)) += std_res;    //????
            k += 2;
            istringstream string_in(std_res);
            string inst;
            string_in >> inst >> inst;    //????????#
            if (inst == "include")
                is_include = 1;
            else if (inst == "define") {
                is_define = 1;
            }
        }

        else if ((str_buffer[k] == '<' || str_buffer[k] == '>' || str_buffer[k] == '|'
                  || str_buffer[k] == '&' || str_buffer[k] == '+' || str_buffer[k] == '-'
                  || str_buffer[k] == ':') && !(is_string || is_char) && !(is_include && is_left_trail)
                 && (k + 1 < str_buffer.size())
                 && (str_buffer[k] == str_buffer[k + 1])) {
            addSpaceToSign(str_buffer, k, 2);
            k += 3;    //????????????????
        }
        else if ((str_buffer[k] == '{' || str_buffer[k] == '[' || str_buffer[k] == '('
                  || str_buffer[k] == ')' || str_buffer[k] == ']' || str_buffer[k] == '}'
                  || str_buffer[k] == ';' || str_buffer[k] == ',' || str_buffer[k] == ':') && !(is_string || is_char)) {
            addSpaceToSign(str_buffer, k, 1); //?????
            k += 2;
        }


            //????+,-,*,/,%??=,<,>, +=,-=,*=,/= ??%=,==,<=.>=
        else if ((str_buffer[k] == '+' || str_buffer[k] == '-' || str_buffer[k] == '*'
                  || str_buffer[k] == '/' || str_buffer[k] == '%' || str_buffer[k] == '='
                  || str_buffer[k] == '<' || str_buffer[k] == '>') && !(is_string || is_char) &&
                 !(str_buffer[k] != '>' && is_include && is_left_trail)) {
            if ((str_buffer[k] == '+' || str_buffer[k] == '-') &&
                (k >= 1 && str_buffer[k - 1] == 'e'))        //?????????
                continue;
            if (str_buffer[k] == '<' && is_include)
                is_left_trail = 1;
            if (str_buffer[k] == '>' && is_include && is_left_trail)
                is_left_trail = is_include = 0;
            if ((k + 1 < str_buffer.size()) && str_buffer[k + 1] == '=') {
                addSpaceToSign(str_buffer, k, 2);
                k += 3;
            } else {
                addSpaceToSign(str_buffer, k, 1);
                k += 2;
            }
        }
             else if ((str_buffer[k] == '~' || str_buffer[k] == '^' || str_buffer[k] == '!'
                  || str_buffer[k] == '|' || str_buffer[k] == '&') && !(is_string || is_char) &&
                 !(is_include && is_left_trail)) {
            addSpaceToSign(str_buffer, k, 1);
            k += 2;
        }

    }

    istringstream str_in(str_buffer);
    vector<string> res = readWordFromStream(str_in);

    if (is_define)
        dealDefineSentence(str_buffer);
    return res;
}
