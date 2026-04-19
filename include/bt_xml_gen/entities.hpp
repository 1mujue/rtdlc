#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "bt_xml_gen/forward.hpp"
class XMLNode{
private:
    std::string tag;
    std::unordered_map<std::string, std::string> attr;
    std::vector<XMLNodeSPtr> children;
public:
    XMLNode(const std::string& tag): tag(tag){}
    void addAttr(const std::string& atname, const std::string& atvalue){
        attr[atname] = atvalue;
    }
    const std::unordered_map<std::string, std::string>& getAttr(){ return attr; }
    void addOAttr(const XMLNode& o){
        for(auto const& it: o.attr){
            attr.emplace(it);
        }
    }
    std::string getAttr(const std::string& key){ 
        if(attr.count(key)) return attr[key];
        return "";
    }
    void addChild(XMLNodeSPtr child) {children.push_back(child);}
    void addOChild(const XMLNode& o){
        for(const auto& it: o.children) children.push_back(it);
    }
    void print(std::ostream& o, int depth){
        printIndent(o, depth);
        o << "<" << tag;
        printAttr(o);
        if(!children.empty()){
            o << ">\n";
            printChildren(o, depth + 1);
            printIndent(o, depth);
            o << "</" << tag << ">\n";
        } else{
            o << "/>\n";
        }
    }
private:
    void printIndent(std::ostream& o, int depth){
        o << std::string(depth * 2, ' ');
    }
    void printAttr(std::ostream& o){
        if(!attr.empty()){
            o << ' ';
            int cnt = 0;
            for(const auto& it: attr){
                o << it.first << " = " << it.second;
                if(++cnt != attr.size()) o << ' ';
            }
        }
    }
    void printChildren(std::ostream& o, int depth){
        for(const auto& it: children) it->print(o, depth);
    }
};