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

using json=nlohmann::json;
using ordered_json=nlohmann::ordered_json;
enum nodeType
{
    leafNode,
    rootNode,
};


struct SyntaxTreeNode {
private:
public:
    std::string name;
    std::string token_name;
    std::vector<SyntaxTreeNode *> child;
    nodeType type;

    ordered_json to_json();

    ~SyntaxTreeNode(){}
};


static void treeNodeClear(SyntaxTreeNode* node){
    // TODO: BUG
//    for(auto &item:node->child)
//        treeNodeClear(item);
//    delete node;
}


class AST
{
public:
    SyntaxTreeNode * root;
    std::string generate_json_file_and_get_json_str(const std::string path= "");
    ordered_json to_json();
    ~AST(){ treeNodeClear(root);}
private:
    ordered_json dfsTraverse(SyntaxTreeNode* curNode);
};


#endif //COMPILER_TREE_H
