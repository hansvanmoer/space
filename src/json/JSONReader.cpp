#include "JSONReader.h"

using namespace JSON;

const int JSONReaderException::NO_LINE = -1;
const int JSONReaderException::NO_COLUMN = -1;


JSONReaderException::JSONReaderException(std::string message) : JSONReaderException(message, NO_LINE, NO_COLUMN){

}

JSONReaderException::JSONReaderException(std::string message, int line, int column) : JSONException(message), line_(line), column_(column){

}

int JSONReaderException::line() const {
    return line_;
}

int JSONReaderException::column() const {
    return column_;
}






