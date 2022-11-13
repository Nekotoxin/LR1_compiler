//
// Created by Neko Toxin on 2022/10/14.
//

#include "Parser.h"
#include <iostream>
#include <utility>
#include <algorithm>
#include <time.h>
#include <sys/stat.h>
#include <stack>
#include <stdexcept>


Parser::Parser(std::string grammar_file) {
    GrammarPreProcess preProcess(grammar_file);
    this->generators = preProcess.generators;
    this->terminals = preProcess.terminals;
    this->non_terminals = preProcess.non_terminals;
    time_t start = clock();
    parsingTable = new ParsingTable(this->generators, this->terminals, this->non_terminals);
    time_t end = clock();
    saveParsingTable("./parse_table.txt", "./parse_table.csv");
    std::cout << "Parsing table generated in " << (double) (end - start) / CLOCKS_PER_SEC << "s" << std::endl;

    std::ofstream out("./timestamp.txt", std::ios::out);
    struct stat result;
    stat(grammar_file.c_str(), &result);
    out << result.st_mtime;
    out.close();
}

Parser::~Parser() {
    if (parsingTable) {
        delete parsingTable;
    }
}

void Parser::Parse(TokenStream &token_stream, AST &tree) {
    time_t start = clock();
    time_t end;
    // 连续比较，文法还不支持
    // state stack
    token_stream.push_back(Token(END_SYMBOL, END_SYMBOL, 0, 0));
    std::stack<StateNo> state_stack;
    // symbol stack
    std::stack<std::string> symbol_stack;
    std::stack<SyntaxTreeNode *> node_stack;
    // TODO: ADD

    auto token_it = token_stream.begin();
    auto state = 0;
    state_stack.push(state);
//    symbol_stack.push(END_SYMBOL);
    Generator generator;
    int generator_size;
    while (true) {
        auto symbol = token_it->token_name;
        Action action;
        StateNo next_state;
        if (parsingTable->action_table.find(std::make_pair(state, symbol)) != parsingTable->action_table.end()) {
            auto table_item = parsingTable->action_table[std::make_pair(state, symbol)];
            action = table_item.first;
            next_state = table_item.second;
        } else if (parsingTable->action_table.find(std::make_pair(state, NULL_SYMBOL)) !=
                   parsingTable->action_table.end()) {
            // 这里对应ParsingTable.cpp中的对于X -> NULL的处理，必须加上猜测X为空的情况
            auto table_item = parsingTable->action_table[std::make_pair(state, NULL_SYMBOL)];
            action = table_item.first;
            next_state = table_item.second;
            // push null symbol
            symbol_stack.push(NULL_SYMBOL);
            // push next state
            state_stack.push(next_state);
            SyntaxTreeNode *rootNode = new SyntaxTreeNode;
            rootNode->type = nodeType::rootNode;
            rootNode->name = NULL_SYMBOL;
            node_stack.push((rootNode));

        } else {
            std::cerr << "❌ Error: " << symbol << " is not expected at state " << state
                      << ", row:" << token_it->row << " col:" << token_it->col << std::endl;
            tree.root = nullptr;
            return;
        }
        // print symbol stack
        std::stack<std::string> tmp_stack;
        std::cout << "[ $ ";
        while (!symbol_stack.empty()) {
            tmp_stack.push(symbol_stack.top());
            symbol_stack.pop();
        }
        while (!tmp_stack.empty()) {
            std::cout << tmp_stack.top() << " ";
            symbol_stack.push(tmp_stack.top());
            tmp_stack.pop();
        }
        std::cout << "  ⬅️  ";
        // print input
        for (auto it = token_it; it != token_stream.end(); it++) {
            std::cout << it->token_name << " ";
        }
        std::cout << "]" << std::endl;
        switch (action) {
            case shift: {
                std::cout << "  ◀️ 预读取符号(lookahead)为： \"" << symbol << "\" ，移进，转到状态 " << next_state
                          << std::endl;
                state_stack.push(next_state);
                symbol_stack.push(symbol);
                auto leafNode = new SyntaxTreeNode;
                leafNode->name = token_it->name;
                leafNode->token_name = symbol;
                leafNode->type = nodeType::leafNode;
                node_stack.push((leafNode));    //加入子节点到结点栈
                token_it++;
                break;
            }
            case reduce: {
                generator = parsingTable->generators[next_state];
                generator_size = generator.second.size();


                std::cout << "  ⏫ 通过生成式 [" << generator.first << " -> ";
                for (auto &i: generator.second) {
                    std::cout << i << " ";
                }
                std::cout << "] 来规约" << std::endl;
                auto rootNode = new SyntaxTreeNode;
                rootNode->type = nodeType::rootNode;
                rootNode->name = generator.first;
                rootNode->token_name = "use generator: ";
                rootNode->token_name += generator.first;
                rootNode->token_name += " -> ";
                for (auto &item: generator.second)
                    (rootNode->token_name += item) += " ";

                for (auto i = 0; i < generator_size; i++) {
                    state_stack.pop();
                    symbol_stack.pop();
                    auto tmpNode = node_stack.top();
                    rootNode->child.insert(rootNode->child.begin(), tmpNode);//倒序设置为子节点
                    node_stack.pop();
                }
                state = state_stack.top();
                symbol_stack.push(generator.first);
                node_stack.push(rootNode);  //加入到根节点
                if (parsingTable->goto_table.find(std::make_pair(state, generator.first)) ==
                    parsingTable->goto_table.end()) {
                    std::cerr << "❌ Error: " << generator.first << " is not expected at state " << state
                              << ", row:" << token_it->row << " col:" << token_it->col << " " << std::endl;
                    tree.root = nullptr;
                    return;
                }
                state_stack.push(parsingTable->goto_table[std::make_pair(state, generator.first)].second);
                break;
            }
            case go:
                std::cout << "转进到状态 " << next_state << std::endl;
                state_stack.push(next_state);
                break;
            case accept:
                tree.root = node_stack.top(); //生成语法树
                std::cout << "✅ Accept" << std::endl;
                end = clock();
                std::cout << "Parse部分耗时：" << (double) (end - start) / CLOCKS_PER_SEC << "s" << std::endl;
                return;
            case error:
                std::cerr << "❌ Error: " << symbol << " is not expected at state " << state
                          << ", row:" << token_it->row << " col:" << token_it->col << std::endl;
                tree.root = nullptr;
                return;
            default:
                break;
        }
        state = state_stack.top();
    }

}

bool Parser::loadParsingTable(const std::string path1, const std::string path2, time_t yacc_file_last_modify_time) {
    if (parsingTable) {
        delete parsingTable;
        parsingTable = nullptr;
    }


    std::ifstream in("./timestamp.txt", std::ios::in);
    auto old_timestamp = 0;
    if (in.is_open()) {
        in >> old_timestamp;
        in.close();
    }
    if (old_timestamp != yacc_file_last_modify_time) {
        return false;
    }

    in.open(path1.c_str(), std::ios::in);
    if (!in.is_open())
        return false;

    char buffer[4096];
    std::string elem;
    istringstream str_in;

    std::set<std::string> rd_terminals;
    std::set<std::string> rd_non_terminals;
    std::vector<Generator> rd_generators;
    ParsingTable_ rd_action;
    ParsingTable_ rd_goto;

    in.getline(buffer, sizeof(buffer));//skip first line
    in.getline(buffer, sizeof(buffer));
    str_in.clear(); //清除状态，重新设置字符串
    str_in.rdbuf()->str(buffer);
    while (str_in >> elem)
        rd_terminals.insert(elem);
    in.getline(buffer, sizeof(buffer));
    in.getline(buffer, sizeof(buffer));
    str_in.clear(); //清除状态，重新设置字符串
    str_in.rdbuf()->str(buffer);
    while (str_in >> elem)
        rd_non_terminals.insert(elem);

    in.getline(buffer, sizeof(buffer));
    while (in.getline(buffer, sizeof(buffer))) {
        istringstream str_in(buffer);
        std::string elem, ept;
        std::vector<std::string> right;
        str_in >> elem >> ept;  //read ->
        while (str_in >> ept)
            right.push_back(ept);
        rd_generators.push_back(make_pair(elem, right));
    }
    in.close();

    std::string buffer2;
    in.open(path2.c_str(), std::ios::in);
    in.getline(buffer, sizeof(buffer));//read symbols
    std::vector<std::string> symbol_arr;
    istringstream symbol_flush(buffer);
    while (std::getline(symbol_flush, elem, ','))
        if (elem.empty() || elem == "STATE")
            continue;
        else if (elem == "\"") {
            symbol_arr.push_back(",");
            std::getline(symbol_flush, elem, ',');
        } else
            symbol_arr.push_back(elem);

    while (in.getline(buffer, sizeof(buffer))) {
        int cnt = 0, state = 0, next_state = 0;
        std::string symbol;
        Action action;
        istringstream tmp_in(buffer);
        while (std::getline(tmp_in, buffer2, ',')) {
            if (buffer2.empty()) {
                cnt++;
                continue;
            }
            if (cnt == 0)
                state = atoi(buffer2.c_str());
            if (cnt > 0) {
                switch (buffer2[0]) {
                    case 'a':
                        action = Action::accept;
                        break;
                    case 's':
                        action = Action::shift;
                        break;
                    case 'r':
                        action = Action::reduce;
                        break;
                    case 'g':
                        action = Action::go;
                        break;
                    default:
                        action = Action::error;
                        break;
                }
                if (action == Action::accept || action == Action::error)
                    next_state = 0;
                else
                    next_state = atoi(buffer2.substr(1).c_str());
                symbol = symbol_arr[cnt - 1];
                if (action == Action::go)
                    rd_goto[std::make_pair(state, symbol)] = std::make_pair(action, next_state);
                else
                    rd_action[std::make_pair(state, symbol)] = std::make_pair(action, next_state);
            }
            cnt++;
        }
    }


    parsingTable = new ParsingTable;
    parsingTable->terminals = rd_terminals;
    parsingTable->non_terminals = rd_non_terminals;
    parsingTable->action_table = rd_action;
    parsingTable->goto_table = rd_goto;
    parsingTable->generators = rd_generators;


    return true;
}

void Parser::saveParsingTable(const std::string path1, const std::string path2) {
    std::ofstream out(path1.c_str(), std::ios::out);
    out << "terminals : " << terminals.size() << std::endl;
    out << "NULL ";
    for (auto &item: terminals)
        out << item << ' ';
    out << std::endl;

    out << "non_terminals : " << non_terminals.size() << std::endl;
    for (auto &item: non_terminals)
        out << item << ' ';
    out << std::endl;

    out << "generator:" << generators.size() << std::endl;
    for (auto &generator: generators) {
        out << generator.first << " -> ";
        auto right = generator.second;
        for (auto &item: right)
            out << item << " ";
        out << std::endl;
    }
    out.close();

    out.open(path2.c_str(), std::ios::out);
    out << "STATE,NULL";
    for (auto &item: terminals)
        if (item == ",")
            out << ",\",\"";
        else
            out << "," << item;
    for (auto &item: non_terminals)
        out << "," << item;
    out << std::endl;

    int state_size = this->parsingTable->lr1_dfa->dfa_state_map.size();
    for (int i = 0; i < state_size; i++) {
        out << i;
        bool is_exist = 0;
        for (auto &state: this->parsingTable->action_table) {
            if (state.first.first == i && state.first.second == NULL_SYMBOL) {
                out << ",";
                switch (state.second.first) {
                    case Action::shift:
                        out << "s" << state.second.second;
                        break;
                    case Action::reduce:
                        out << "r" << state.second.second;
                        break;
                    case Action::accept:
                        out << "acc";
                        break;
                    case Action::go:
                        out << "g" << state.second.second;
                        break;
                }
                is_exist = 1;
                break;
            }
        }
        if (!is_exist)
            out << ",";
        for (auto &item: this->terminals) {
            is_exist = 0;
            for (auto &state: this->parsingTable->action_table) {
                if (state.first.first == i && state.first.second == item) {
                    out << ",";
                    switch (state.second.first) {
                        case Action::shift:
                            out << "s" << state.second.second;
                            break;
                        case Action::reduce:
                            out << "r" << state.second.second;
                            break;
                        case Action::accept:
                            out << "acc";
                            break;
                        case Action::go:
                            out << "g" << state.second.second;
                            break;
                    }
                    is_exist = 1;
                    break;
                }
            }
            if (!is_exist)
                out << ",";
        }
        for (auto &item: this->non_terminals) {
            bool is_exist = 0;
            for (auto &state: this->parsingTable->goto_table) {
                if (state.first.first == i && state.first.second == item) {
                    out << ",";
                    switch (state.second.first) {
                        case Action::shift:
                            out << "s" << state.second.second;
                            break;
                        case Action::reduce:
                            out << "r" << state.second.second;
                            break;
                        case Action::accept:
                            out << "acc" << state.second.second;
                            break;
                        case Action::go:
                            out << "g" << state.second.second;
                            break;
                    }
                    is_exist = 1;
                    break;
                }
            }
            if (!is_exist)
                out << ",";
        }
        out << std::endl;
    }
    out.close();
}
