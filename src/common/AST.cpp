//
// Created by Neko Toxin on 2022/10/14.
//

#include "AST.h"
#include<sstream>
#include<iostream>


ordered_json AST::dfsTraverse(SyntaxTreeNode *curNode)
{
    ordered_json res=curNode->to_json();
    std::cout<<res.dump(4)<<std::endl;
    ordered_json j_child=json::array();
    for(auto &item:curNode->child)
        j_child.push_back(dfsTraverse(item));

    res["children"]=j_child;
    return res;
}

std::string AST::generate_json_file_and_get_json_str(const std::string path) {
    ordered_json res= dfsTraverse(root);
    if(path!="") {
        std::ofstream out(path.c_str(), std::ios::out);
        out << res.dump(4);
        out.close();
    }
    std::ostringstream  str;
    str<<res.dump(4);
    return str.str();
}

ordered_json AST::to_json() {
    return dfsTraverse(root);
}

ordered_json SyntaxTreeNode::to_json() {
    std::string type=(this->type==nodeType::leafNode?"terminal":"Symbol");
    ordered_json res;
    res["name"]=this->name;
    ordered_json  attributes;
    attributes["token_name"]=this->token_name;
    //attributes["type"]=type;
    res["attributes"]=attributes;
    return res;
}




