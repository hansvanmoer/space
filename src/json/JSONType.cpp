#include "JSONType.h"

using namespace JSON;

JSONException::JSONException(std::string msg) : std::runtime_error(msg){}


std::ostream &JSON::operator<<(std::ostream &output, const NodeType &type) {
    switch (type) {
        case NodeType::ARRAY:
            output << "array";
            break;
        case NodeType::BOOLEAN:
            output << "boolean";
            break;
        case NodeType::NULL_VALUE:
            output << "null";
            break;
        case NodeType::NUMBER:
            output << "number";
            break;
        case NodeType::OBJECT:
            output << "object";
            break;
        case NodeType::STRING:
            output << "string";
            break;
    }
    return output;
}