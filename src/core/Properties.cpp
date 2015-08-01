#include "Properties.h"

using namespace Core;

PropertyException::PropertyException(std::string message) : std::runtime_error{message}{

}

ConvertPropertyException::ConvertPropertyException(std::string message) : PropertyException(message) {

}

PropertyNotFoundException::PropertyNotFoundException(std::string message) : PropertyException(message) {

}

PropertySyntaxException::PropertySyntaxException(std::string message, int line) : PropertyException(message), line_(line){

}