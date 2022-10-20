//
// Created by Neko Toxin on 2022/10/17.
//

#ifndef PARSER_LR1DFA_H
#define PARSER_LR1DFA_H

#include <map>
#include <string>
#include <set>
#include "PreProcess.h"
#include <iostream>


typedef int StateNo;
typedef int DotPos;
#include <functional>
#include <utility>
// FIRST set and FOLLOW set
typedef std::map<std::string, std::set<std::string>> FirstSet;
typedef std::map<std::string, std::set<std::string>> FollowSet;



// generator with dot position, such as A -> .aBc, A -> a.Bc
class LR1Item {
public:
    Generator generator;
    DotPos dot_pos;
    std::string lookahead;
    std::size_t hash;
    LR1Item(Generator generator_i, DotPos dot_pos_i, std::string lookahead_i) : generator(std::move(generator_i)),
                                                                                dot_pos(dot_pos_i),
                                                                                lookahead(std::move(lookahead_i)) {
        std::string str = generator.first;
        for (auto &it : generator.second) {
            str += it;
        }
        str += std::to_string(dot_pos);
        str += lookahead;
        std::hash<std::string> hash_str;
        hash = hash_str(str);
    }

};
typedef std::set<LR1Item> DFANode;

typedef std::set<DFANode> DFANodeSet;
class DFAEdge {
public:
    DFANode src;
    std::string eat;
    DFANode dst;

    DFAEdge(DFANode _src, std::string _eat, DFANode _dst) : src(std::move(_src)), eat(std::move(_eat)), dst(std::move(_dst)) {
        std::hash<std::size_t> hash_hash;
//        hash = hash_hash(src.hash + dst.hash);
    }
};

typedef std::set<DFAEdge> DFAEdgeSet;

//typedef std::map<std::pair<DFANode, std::string>, DFANode> DFAEdgeSet;

inline bool operator<(const LR1Item &a, const LR1Item &b) {
    return a.hash < b.hash;
}

inline bool operator==(const LR1Item &a, const LR1Item &b) {
    return a.hash == b.hash;
}

inline bool operator!=(const LR1Item &a, const LR1Item &b) {
    return !(a == b);
}

inline bool operator<(const DFAEdge &a, const DFAEdge &b) {
    if (a.eat != b.eat) {
        return a.eat < b.eat;
    }else if (a.src != b.src) {
        return a.src < b.src;
    }  else {
        return a.dst < b.dst;
    }
    return false;
}


class LR1DFA {
public:
    // 生成式，终结符和非终结符
    const std::vector<Generator>& generators;
    const std::set<std::string>& terminals;
    const std::set<std::string>& non_terminals;
    // FIRST集, FOLLOW集
    FirstSet first_set;
    FollowSet follow_set;
    std::set<std::string> nullable_set;
    // start set and start symbol
    DFANode start_set;
    std::string start_symbol;
    DFANode* dfa_nodes;
    std::map<StateNo, DFANode*> dfa_state_map;
    std::map<DFANode*, StateNo> dfa_state_map_reverse;
    std::map<StateNo, std::map<std::string, StateNo>> dfa_transition_map;
    // transition function

    LR1DFA(const std::vector<Generator>& generators, const std::set<std::string>& terminals,
           const std::set<std::string>& non_terminals);

    void ConstructFirstSetAndFollowSet();

    void ConstructLR1DFA();

    void GetClosure(std::set<LR1Item> &state_set);

    void GetGoto(const std::set<LR1Item> &state_set, const std::string& X, std::set<LR1Item> &goto_set);

    void PrintMemberVars();

    bool IsTerminal(std::string symbol);


};


#endif //PARSER_LR1DFA_H
