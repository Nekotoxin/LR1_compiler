//
// Created by Neko Toxin on 2022/10/15.
//

#include "ParsingTable.h"
#include <algorithm>
#include <utility>


ParsingTable::ParsingTable(const std::vector<Generator> &_generators, const std::set<std::string> &_terminals,
                           const std::set<std::string> &_non_terminals) : generators(_generators),
                                                                          terminals(_terminals),
                                                                          non_terminals(_non_terminals) {

    lr1_dfa = new LR1DFA(generators, terminals, non_terminals);
    ConstructParsingTable();
}


void ParsingTable::ConstructParsingTable() {
    // print state
    // construct parsing table
    for (auto &item: lr1_dfa->dfa_state_map) {
        auto state_no = item.first;
        auto state_set = *(item.second);
        //查找该状态中是否有 . 在最后的状态，如果有先根据向前搜索符确定哪一列，再用 rn，填入表示，r 的含义是规约，n 表示的是产生式的序号；如果没有则说明没有没有 r
//        int is_reduce = 0;
        for (auto &lr1_item: state_set) {
            if (lr1_item.dot_pos == lr1_item.generator.second.size()) {
                auto lookahead = lr1_item.lookahead;
                auto lval = lr1_item.generator.first;
//                LR1ItemSet next_state = dfa_edge_set[std::make_pair(state_set, lookahead)];
//                auto next_state_no = dfa_state_map_reverse[next_state];
                auto generator_no =
                        std::find(generators.begin(), generators.end(), lr1_item.generator) - generators.begin();
                if (action_table.find(std::make_pair(state_no, lookahead)) != action_table.end()) {
                    auto res = action_table[std::make_pair(state_no, lookahead)];
                    std::cerr << "error: shift/reduce conflict" << std::endl;
//                    std::cerr << "now state: " << state_no << std::endl;
//                    std::cerr << "now lookahead: " << lookahead << std::endl;
//                    std::cerr << "pre action: " << (char)res.first << std::endl;
//                    std::cerr << "pre next state: " << res.second << std::endl;
//                    std::cerr << "now action: " << (char)Action::reduce << std::endl;
//                    std::cerr << "generator no: " << generator_no << std::endl;
                    exit(-1);
                }
                action_table[std::make_pair(state_no, lookahead)] = std::make_pair(Action::reduce, generator_no);
            } else {
                auto lookahead = lr1_item.lookahead;
                auto X = lr1_item.generator.second[lr1_item.dot_pos];
                DFANode next_state = {};
                if (X == NULL_SYMBOL) {
                    continue;
                }
                auto next_state_no = lr1_dfa->dfa_transition_map[state_no][X];
//                lr1_dfa->GetGoto(state_set, X, next_state);
                if (X == END_SYMBOL) {
                    action_table[std::make_pair(state_no, X)] = std::make_pair(Action::accept, 0);
                } else if (lr1_dfa->IsTerminal(X)) {
                    if (action_table.find(std::make_pair(state_no, X)) != action_table.end()) {
                        auto res = action_table[std::make_pair(state_no, X)];
                        if (res.first != Action::shift || res.second != next_state_no) {
                            std::cerr << "error: shift/reduce conflict" << std::endl;
//                            std::cerr << "now state: " << state_no << std::endl;
//                            std::cerr << "now lookahead: " << X << std::endl;
//                            std::cerr << "pre action: " <<(char) res.first << std::endl;
//                            std::cerr << "pre next state: " << res.second << std::endl;
//                            std::cerr << "now action: " << (char)Action::shift << std::endl;
//                            std::cerr << "now next state: " << next_state_no << std::endl;
                            exit(-1);
                        }
                    }
                    action_table[std::make_pair(state_no, X)] = std::make_pair(Action::shift, next_state_no);
                } else {
                    if (goto_table.find(std::make_pair(state_no, X)) != goto_table.end()) {
                        auto res = goto_table[std::make_pair(state_no, X)];
                        if (res.first != Action::go || res.second != next_state_no) {
                            std::cerr << "error: shift/reduce conflict" << std::endl;
//                            std::cerr << "error: shift/reduce conflict" << std::endl;
//                            std::cerr << "now state: " << state_no << std::endl;
//                            std::cerr << "now lookahead: " << X << std::endl;
//                            std::cerr << "pre next state: " << res.second << std::endl;
//                            std::cerr << "now next state: " << next_state_no << std::endl;
                            exit(-1);
                        }
                    }
                    goto_table[std::make_pair(state_no, X)] = std::make_pair(Action::go, next_state_no);
                    // if nullable, add null to action table
                    // add reduce rule: NULL -> X
                    // 特殊处理，猜测非终结符X可能为空，否则无法兼容形为 S -> { X } , X -> NULL 的文法
                    if (lr1_dfa->nullable_set.find(X) != lr1_dfa->nullable_set.end()) {
                        auto generator_tmp = std::make_pair(X, std::vector<std::string>{NULL_SYMBOL});
                        auto generator_no =
                                std::find(generators.begin(), generators.end(), generator_tmp) - generators.begin();
                        action_table[std::make_pair(state_no, NULL_SYMBOL)] = std::make_pair(Action::reduce,
                                                                                             generator_no);
                    }
                }
            }
        }
    }
    //print action table
    std::cout << std::endl << "Action Table" << std::endl;
    std::cout << lr1_dfa->dfa_state_map.size() << std::endl;
    std::cout << this->action_table.size() << std::endl;
    // state_no: terminal1 action1, terminal2 action2, ...
    int state_size = lr1_dfa->dfa_state_map.size();
    for (int i = 0; i < state_size; ++i) {
        std::cout << "State" << i << ": ";
        for (auto &item: action_table) {
            if (item.first.first == i) {
                switch (item.second.first) {
                    case Action::shift:
                        std::cout << item.first.second << " " << "s" << item.second.second << ", ";
                        break;
                    case Action::reduce:
                        std::cout << item.first.second << " " << "r" << item.second.second << ", ";
                        break;
                    case Action::accept:
                        std::cout << item.first.second << " " << "acc" << ", ";
                        break;
                    default:
                        break;
                }
            }
        }
        std::cout << std::endl;
    }
    //print goto table
    std::cout << std::endl << "Goto Table" << std::endl;
    // state_no: nonterminal1 action1, nonterminal2 action2, ...
    for (int i = 0; i < state_size; ++i) {
        std::cout << "State" << i << ": ";
        for (auto &item: goto_table) {
            if (item.first.first == i) {
                std::cout << item.first.second << " " << (char) item.second.first << item.second.second << ", ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

ParsingTable::~ParsingTable() {
    if(lr1_dfa)
    delete lr1_dfa;
}



