#include "JSONReader.h"

using namespace JSON;

const int ReaderException::NO_LINE = -1;
const int ReaderException::NO_COLUMN = -1;

std::string createErrorMessage(const std::string &msg, int line, int column){
    if(line == ReaderException::NO_LINE || column == ReaderException::NO_COLUMN){
        return msg;
    }else{
        std::ostringstream buffer;
        buffer << msg << " at line " << line << ", column " << column;
        return buffer.str();
    }
}

ReaderException::ReaderException(std::string message) : JSONException(message), line_(NO_LINE), column_(NO_COLUMN){

}

ReaderException::ReaderException(std::string message, int line, int column) : JSONException(createErrorMessage(message, line, column)), line_(line), column_(column){

}

int ReaderException::line() const {
    return line_;
}

int ReaderException::column() const {
    return column_;
}






