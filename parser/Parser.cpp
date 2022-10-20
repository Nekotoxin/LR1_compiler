//
// Created by Neko Toxin on 2022/10/14.
//

#include "Parser.h"
#include <iostream>
#include <utility>
#include <algorithm>
#include <time.h>
#include <stack>


Parser::Parser(std::string source_code_from_lexer_file, std::string grammar_file) {
    PreProcess preProcess(source_code_from_lexer_file, grammar_file);
    time_t start = clock();
    parsingTable = new ParsingTable(preProcess.generators, preProcess.terminals, preProcess.non_terminals);
    time_t end = clock();
    std::cout << "Parsing table generated in " << (double) (end - start) / CLOCKS_PER_SEC << "s" << std::endl;
    Parse();
}

Parser::~Parser() {
    delete parsingTable;
}

void Parser::Parse() {
    time_t start = clock();
    time_t end;
    // 连续比较，文法还不支持
    std::string input_before = "int ID ( ) { if ( num + num <= ID / num ) { { } int ID = num * num ; } else if ( ID ) { float ID ; } else if ( num ) { int ID ; ID = num ; int ID ; } else { } float ID ; int ID ; ID = num ; ID = num + num ; int ID ; ID = ( num * num + ID - ID / num + ID / ( ID - num ) ) ; while ( num + ID ( num , ID , num + ID ) ) { int ID ; while ( num ) { int ID ; ID = num ; { } } } if ( ID + num * ( ID * num - ID ) ) { int ID ; ID = num ; return ID + num ; } else { return num * ID ; } int ID ; return num ; }";
    std::vector<std::string> input = {};
    // 按照空格分割input_before
    std::string tmp = "";
    for (auto &ch: input_before) {
        if (ch == ' ') {
            input.push_back(tmp);
            tmp = "";
        } else {
            tmp += ch;
        }
    }
    input.push_back(tmp);
    input.push_back(END_SYMBOL);
    // state stack
    std::stack<StateNo> state_stack;
    // symbol stack
    std::stack<std::string> symbol_stack;

    auto input_idx = 0;
    auto state = 0;
    state_stack.push(state);
//    symbol_stack.push(END_SYMBOL);
    Generator generator;
    int generator_size;
    while (true) {
        auto symbol = input[input_idx];
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
        } else {
            std::cout << "❌ Error: " << symbol << " is not expected at state " << state << std::endl;
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
        for (auto i = input_idx; i < input.size(); ++i) {
            std::cout << input[i] << " ";
        }
        std::cout <<"]"<< std::endl;

        switch (action) {
            case shift:
                std::cout << "  ◀️ 预读取符号(lookahead)为： \"" << symbol << "\" ，移进，转到状态 " << next_state << std::endl;
                state_stack.push(next_state);
                symbol_stack.push(symbol);
                input_idx++;
                break;
            case reduce:
                generator = parsingTable->generators[next_state];
                generator_size = generator.second.size();

                std::cout << "  ⏫ 通过生成式 [" << generator.first << " -> ";
                for (auto &i: generator.second) {
                    std::cout << i << " ";
                }
                std::cout << "] 来规约" << std::endl;

                for (auto i = 0; i < generator_size; i++) {
                    state_stack.pop();
                    symbol_stack.pop();
                }
                state = state_stack.top();
                symbol_stack.push(generator.first);
                if (parsingTable->goto_table.find(std::make_pair(state, generator.first)) ==
                    parsingTable->goto_table.end()) {
                    std::cout << "⛔ Error: " << generator.first << " is not expected at state " << state << std::endl;
                    return;
                }
                state_stack.push(parsingTable->goto_table[std::make_pair(state, generator.first)].second);
                break;
            case go:
                std::cout << "转进到状态 " << next_state << std::endl;
                state_stack.push(next_state);
                break;
            case accept:
                std::cout << "✅ Accept" << std::endl;
                end = clock();
                std::cout << "Parse部分耗时：" << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;
                return;
            case error:
                std::cout << "⛔ Error" << std::endl;
                return;
            default:
                break;
        }
        state = state_stack.top();
    }


}
