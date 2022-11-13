//
// Created by zhangxun on 2022/11/8.
//

#ifndef COMPILER_TREE_H
#define COMPILER_TREE_H

#include<string>
#include"../parser/GrammarPreProcess.h"
#include<vector>
#include<fstream>
#include<json.hpp>

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
enum nodeType {
    leafNode,
    rootNode,
};


struct SyntaxTreeNode {
public:
    std::string name;
    std::string token_name;
    std::vector<SyntaxTreeNode *> child;
    nodeType type;

    ordered_json to_json();
};


static void treeNodeClear(SyntaxTreeNode *node) {
    if (node == nullptr) return;
    for (auto item: node->child)
        treeNodeClear(item);
    auto name = node->name;
    if (node) {
        delete node;
        node = nullptr;
    }
}


class AST {
public:
    SyntaxTreeNode *root;

    ordered_json to_json();

    AST() { root = nullptr; }

    ~AST() { treeNodeClear(root); }

private:
    ordered_json dfsTraverse(SyntaxTreeNode *curNode);
};


#endif //COMPILER_TREE_H
