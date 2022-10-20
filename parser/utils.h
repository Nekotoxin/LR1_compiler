#ifndef PARSAR_UTILS_H
#define PARSAR_UTILS_H

#include <string>
#include <vector>

// 清除一个字符串首位空格和制表符
std::string trim(std::string str) {
    if (str.empty()) {
        return str;
    }
    // skip all space and \t
    int i = 0;
    while (str[i] == ' ' || str[i] == '\t') {
        i++;
    }
    str = str.substr(i);
    // skip all space and \t
    i = str.size() - 1;
    while (str[i] == ' ' || str[i] == '\t') {
        i--;
    }
    str = str.substr(0, i + 1);
    return str;
}

std::vector<std::string> split(std::string str, char delimiter) {
    std::vector<std::string> res;
    std::string temp;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == delimiter) {
            if (temp.length()) {
                res.push_back(temp);
            }

            temp = "";
        } else {
            temp += str[i];
        }
    }
    res.push_back(temp);
    return res;

}


#endif //PARSAR_UTILS_H