//
// Created by Neko Toxin on 2022/10/17.
//

#include "LR1DFA.h"
#include <algorithm>

void printLR1ItemSet(const std::string msg, const std::set<LR1Item> &lr1ItemSet) {
    std::cout << msg << std::endl;
    for (auto &lr1Item: lr1ItemSet) {
        // print left
        std::cout << lr1Item.generator.first << " -> ";
        // print dot as '.' at dot_pos
        for (int i = 0; i < lr1Item.generator.second.size(); i++) {
            if (i == lr1Item.dot_pos) {
                std::cout << ". ";
            }
            std::cout << lr1Item.generator.second[i] << " ";
        }
        if (lr1Item.dot_pos == lr1Item.generator.second.size()) {
            std::cout << ". ";
        }
        std::cout << ", " << lr1Item.lookahead << std::endl;
    }
}

int count_set_total_size(std::map<std::string, std::set<std::string>> &s) {
    int total_size = 0;
    for (auto &item: s) {
        total_size += item.second.size();
    }
    return total_size;
}

LR1DFA::LR1DFA(const std::vector<Generator> &_generators, const std::set<std::string> &_terminals,
               const std::set<std::string> &_non_terminals) : generators(_generators), terminals(_terminals),
                                                              non_terminals(_non_terminals) {
    if (generators.empty()) {
        std::cout << "generators is empty" << std::endl;
        exit(1);
    }
    ConstructFirstSetAndFollowSet();
    PrintMemberVars();
    std::cout<<"\n别急，正在构造LR(1) DFA\n";
    ConstructLR1DFA();
}

void LR1DFA::GetClosure(std::set<LR1Item> &state_set) {
//    using namespace std;
    int closure_size = 0;
    do {
        closure_size = state_set.size();
        for (auto &item: state_set) {
            std::set<std::string> first_set_of_beta_z;
            // get first set of beta_z
            // A -> a.Xb, z
            auto beta_pos = item.dot_pos + 1;
            int is_all_nullable = 1;
            for (int i = beta_pos; i < item.generator.second.size(); i++) {
                auto beta = item.generator.second[i];
                if (terminals.find(beta) != terminals.end()) {
                    first_set_of_beta_z.insert(beta);
                    is_all_nullable = 0;
                    break;
                } else if (nullable_set.find(beta) == nullable_set.end()) {
                    is_all_nullable = 0;
                }
                for (auto &item2: first_set[beta]) {
                    first_set_of_beta_z.insert(item2);
                }
            }
            // if all beta_z are nullable, add lookahead to first_set_of_beta_z
            if (is_all_nullable) {
                if (item.lookahead == ANY_SYMBOL) {
                    // insert all terminals
                    for (auto &item2: terminals) {
                        first_set_of_beta_z.insert(item2);
                    }
                } else {
                    first_set_of_beta_z.insert(item.lookahead);
                }
            }
            // if X is a non-terminal
            if (item.dot_pos >= item.generator.second.size()) continue;
            std::string dot_pos_str = item.generator.second[item.dot_pos];
            for (auto &generator: generators) {
                std::string l_val = generator.first;
                if (l_val != dot_pos_str) continue;
                // any w in first_set_of_beta_z, add X -> .y, w to state_set
                for (auto &w: first_set_of_beta_z) {
                    LR1Item new_item = {generator, 0, w};
                    state_set.insert(new_item);
                }
            }
        }
    } while (closure_size != state_set.size());
}


void LR1DFA::ConstructFirstSetAndFollowSet() {// construct FIRST and FOLLOW set
    for (auto &terminal: terminals) {
        first_set[terminal].insert(terminal);
    }
    // construct nullable set
    for (auto &generator: generators) {
        auto r_val = generator.second;
        if (r_val.size() == 1 && r_val[0] == NULL_SYMBOL) {
            nullable_set.insert(generator.first);
//            generator.second = {""};
        }
    }

    // construct first and follow set
    int first_set_size = 0;
    int follow_set_size = 0;
    int nullable_set_size = 0;
    do {
        first_set_size = count_set_total_size(first_set);
        follow_set_size = count_set_total_size(follow_set);
        nullable_set_size = nullable_set.size();
        for (auto &generator: generators) {
            auto r_val = generator.second;
            int is_all_y_i_nullable = 1;
            int k = r_val.size();
            for (int i = 0; i < k; i++) {
                if (nullable_set.find(r_val[i]) == nullable_set.end()) {
                    is_all_y_i_nullable = 0;
                    break;
                }
            }

            if (is_all_y_i_nullable) {
                nullable_set.insert(generator.first);
            }

            for (int i = 0; i < k; i++) {
                int is_y_1_to_y_i_minus_1_nullable = 1;
                for (int j = 0; j < i; j++) {
                    if (nullable_set.find(r_val[j]) == nullable_set.end()) {
                        is_y_1_to_y_i_minus_1_nullable = 0;
                        break;
                    }
                }
                if (is_y_1_to_y_i_minus_1_nullable) {
                    first_set[generator.first].insert(first_set[r_val[i]].begin(), first_set[r_val[i]].end());
                }

                int is_y_i_plus_1_to_y_k_nullable = 1;
                for (int j = i + 1; j < k; j++) {
                    if (nullable_set.find(r_val[j]) == nullable_set.end()) {
                        is_y_i_plus_1_to_y_k_nullable = 0;
                        break;
                    }
                }
                if (is_y_i_plus_1_to_y_k_nullable) {
                    follow_set[r_val[i]].insert(follow_set[generator.first].begin(),
                                                follow_set[generator.first].end());
                }

                for (int j = i + 1; j < k; j++) {
                    int is_y_i_plus_1_to_y_j_minus_1_nullable = 1;
                    for (int l = i + 1; l < j; l++) {
                        if (nullable_set.find(r_val[l]) == nullable_set.end()) {
                            is_y_i_plus_1_to_y_j_minus_1_nullable = 0;
                            break;
                        }
                    }
                    if (is_y_i_plus_1_to_y_j_minus_1_nullable) {
                        follow_set[r_val[i]].insert(first_set[r_val[j]].begin(), first_set[r_val[j]].end());
                    }
                }

            }
        }
    } while (first_set_size != count_set_total_size(first_set) || follow_set_size != count_set_total_size(follow_set) ||
             nullable_set_size != nullable_set.size());
}


void LR1DFA::PrintMemberVars() {
    // print all result
    std::cout << "FIRST SET(没有把epsilon加到first集中，直接用nullable_set管理):" << std::endl;
    for (auto &item: first_set) {
        // only display non-terminals
        if (non_terminals.find(item.first) != non_terminals.end()) {
            std::cout << item.first << ": [";
            for (auto &item2: item.second) {
                std::cout << item2 << " ";
            }
            std::cout << "]" << std::endl;
        }
    }

    std::cout << std::endl;

    std::cout << "FOLLOW SET:" << std::endl;
    for (auto &item: follow_set) {
        if (non_terminals.find(item.first) != non_terminals.end()) {
            std::cout << item.first << ": [";
            for (auto &item2: item.second) {
                std::cout << item2 << " ";
            }
            std::cout << "]" << std::endl;
        }
    }

    std::cout << std::endl;
    std::cout << "NULLABLE SET:" << std::endl;
    std::cout << "[";
    for (auto &item: nullable_set) {
        std::cout << item << " ";
    }
    std::cout << "]" << std::endl << std::endl;

    // print all generators
    std::cout << "GENERATORS:" << std::endl;
    for (auto &generator: generators) {
        std::cout << generator.first << " -> ";
        auto r_val = generator.second;
        for (auto &item: r_val) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
}

void LR1DFA::GetGoto(const std::set<LR1Item> &state_set, const std::string &eat, std::set<LR1Item> &goto_set) {
    goto_set.clear();
    time_t start = clock();
    for (auto &item: state_set) {
        if (item.generator.second[item.dot_pos] == eat) {
            int new_dot_pos = item.dot_pos + 1;
            if (new_dot_pos > item.generator.second.size()) continue;
            LR1Item new_item = {item.generator, new_dot_pos, item.lookahead};
            goto_set.insert(new_item);
        }
    }

    GetClosure(goto_set);
}

void LR1DFA::ConstructLR1DFA() {
    start_set = {};
    // find all generators with start symbol

    auto start_symbol = generators[0].first;
    for (auto &generator: generators) {
        if (generator.first == start_symbol) {
            LR1Item item = {generator, 0, ANY_SYMBOL};
            start_set.insert(item);
        }
    }
    GetClosure(start_set);

    DFANodeSet dfa_node_set = {};
    DFAEdgeSet dfa_edge_set = {};
    dfa_node_set.insert(start_set);

    int node_set_size = dfa_node_set.size();
    int edge_set_size = dfa_edge_set.size();
    double time_count_1 = 0;
    double time_count_2 = 0;
    do {
        node_set_size = dfa_node_set.size();
        edge_set_size = dfa_edge_set.size();
        for (auto &dfa_node: dfa_node_set) {
//            std::cout<<"node_set_size: "<<dfa_node_set.size()<<std::endl;
            for (auto &lr1_item: dfa_node) {
                if (lr1_item.dot_pos == lr1_item.generator.second.size()) continue;
                std::string X = lr1_item.generator.second[lr1_item.dot_pos];
                if (X == END_SYMBOL || X == NULL_SYMBOL) continue;
                DFANode new_node_from_goto = {};
                time_t start_time = clock();
                GetGoto(dfa_node, X, new_node_from_goto);
                time_count_1 += clock() - start_time;
                if (new_node_from_goto.empty()) continue;
                time_t start_time_2 = clock();
                dfa_node_set.insert(new_node_from_goto);
                DFAEdge edge = {dfa_node, X, new_node_from_goto};
                dfa_edge_set.insert(edge);
                time_count_2 += clock() - start_time_2;
            }
        }
    } while (node_set_size != dfa_node_set.size() || edge_set_size != dfa_edge_set.size());
    std::cout << "time_count_1: " << time_count_1 / CLOCKS_PER_SEC << std::endl;
    std::cout << "time_count_2: " << time_count_2 / CLOCKS_PER_SEC << std::endl;

    std::cout << "dfa_node_set.size() = " << dfa_node_set.size() << std::endl;
    // set the start state to no.1
    // copy the dfa_node_set to dfa_nodes
    int nodes_size = dfa_node_set.size();
    dfa_nodes = new DFANode[nodes_size];
    DFANode *p_start;
    int count = 0;
    for (auto &dfa_node: dfa_node_set) {
        dfa_nodes[count] = dfa_node;
        if (dfa_node == start_set) {
            p_start = &dfa_nodes[count];
        }
        count++;
    }


    StateNo state_no = 0;
    dfa_state_map[state_no] = p_start;
    dfa_state_map_reverse[p_start] = state_no;
    for (int idx = 0; idx < nodes_size; idx++) {
        DFANode *p_cur = &dfa_nodes[idx];
        if (p_cur == p_start) {
            continue;
        }
        dfa_state_map[++state_no] = p_cur;
        dfa_state_map_reverse[p_cur] = state_no;
    }

    std::cout << dfa_state_map.size() << std::endl;

    // construct the dfa_transition_map
    for (auto &dfa_edge: dfa_edge_set) {
        auto node1 = dfa_edge.src;
        auto node2 = dfa_edge.dst;
        auto p_node1 = std::find(dfa_nodes, dfa_nodes + nodes_size, node1);
        auto p_node2 = std::find(dfa_nodes, dfa_nodes + nodes_size, node2);
        StateNo state_no1 = dfa_state_map_reverse[p_node1];
        StateNo state_no2 = dfa_state_map_reverse[p_node2];
        dfa_transition_map[state_no1][dfa_edge.eat] = state_no2;
    }

    for (auto &item: dfa_state_map) {
        std::string msg;
        msg = "State" + std::to_string(item.first) + ": ";
        printLR1ItemSet(msg, *(item.second));
    }
}

bool LR1DFA::IsTerminal(std::string symbol) {
    return terminals.find(symbol) != terminals.end();
}
