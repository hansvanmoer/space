#include "StringBundle.h"

#include "String.h"
#include "CharacterBuffer.h"

using namespace Core;

StringBundle::StringBundle() {
}

const char32_t *StringBundle::parseStatement(const char32_t *current, const char32_t *end, int line) {
    const char32_t * keyBegin{current};
    const char32_t * keyEnd{};
    while (current != end && (!keyEnd)) {
        std::char_traits<char32_t>::int_type c = std::char_traits<char32_t>::to_int_type(*current);
        if (c > 127) {
            throw ResourceException(toString("invalid character in key at line ", line));
        } else if (std::char_traits<char32_t>::eq_int_type(c, '=')) {
            keyEnd = current;
        } else if (std::char_traits<char32_t>::eq_int_type(c, '\n')) {
            throw ResourceException(toString("unexpected newline in key at line ", line));
        }
        ++current;
    }
    if (keyEnd && current != end) {
        while (current != end && (!std::char_traits<char32_t>::eq(*current, '\n'))) {
            ++current;
        }
        ResourceBundle<std::string, Unicode::String>::add(std::string{keyBegin, keyEnd}, new Unicode::String{keyEnd + 1, current});
        if (current != end) {
            ++current;
        }
        return current;
    } else {
        throw ResourceException{toString("unexpected end of input at line ", line)};
    }
}

void StringBundle::parse(const char32_t *buffer, std::streamsize length) {
    const char32_t *end = buffer + length;
    int line{0};
    while (buffer != end) {
        buffer = parseStatement(buffer, end, line);
        ++line;
    }
}

void StringBundle::load(std::istream &input) {
    if (input.bad()) {
        throw ResourceException("unable to read resources from stream");
    }
    UTF8ToUTF32InputBuffer<char32_t, char> buffer;
    if (buffer.read(input) == BufferState::OK) {//TODO:input.eof()){
        parse(buffer.begin(), buffer.length());
    } else {
        throw ResourceException("an error occurred reading resource from stream ");
    }
}

StringBundle::~StringBundle() {
}


