/* 
 * File:   JSONTree.cpp
 * Author: hans
 * 
 * Created on 30 December 2014, 13:47
 */

#include "JSONTree.h"

#include <sstream>

using namespace JSON;

TypeException::TypeException(NodeType expected, NodeType actual, std::string msg) : JSONException(msg), expected_(expected), actual_(actual) {
};

TypeException::TypeException(NodeType expected, NodeType actual) : TypeException(expected, actual, createMessage(expected, actual)) {
};

NodeType TypeException::expected() const {
    return expected_;
};

NodeType TypeException::actual() const {
    return actual_;
};

std::string TypeException::createMessage(NodeType expected, NodeType actual) {
    std::ostringstream buffer;
    buffer << "wrong node type, expected '" << expected << "', but got '" << actual << "'";
    return buffer.str();
};