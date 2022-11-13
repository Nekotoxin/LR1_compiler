//
// Created by Neko Toxin on 2022/10/14.
//

#include "AST.h"
#include<sstream>
#include<iostream>


ordered_json AST::dfsTraverse(SyntaxTreeNode *curNode) {
    if (curNode == nullptr)
        return nullptr;
    ordered_json res = curNode->to_json();
    ordered_json j_child = json::array();
    for (auto &item: curNode->child)
        j_child.push_back(dfsTraverse(item));

    res["children"] = j_child;
    return res;
}

ordered_json AST::to_json() {
    return dfsTraverse(root);
}

ordered_json SyntaxTreeNode::to_json() {
    ordered_json res;
    res["name"] = this->name;
    ordered_json attributes;
    attributes["token_name"] = this->token_name;
    res["attributes"] = attributes;
    return res;
}




