#include "JSONReader.h"

using namespace JSON;

const int ReaderException::NO_LINE = -1;
const int ReaderException::NO_COLUMN = -1;


ReaderException::ReaderException(std::string message) : ReaderException(message, NO_LINE, NO_COLUMN){

}

ReaderException::ReaderException(std::string message, int line, int column) : JSONException(message), line_(line), column_(column){

}

int ReaderException::line() const {
    return line_;
}

int ReaderException::column() const {
    return column_;
}






