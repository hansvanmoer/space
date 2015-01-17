/* 
 * File:   JSONParser.h
 * Author: hans
 *
 * Created on 30 December 2014, 20:12
 */

#ifndef JSONPARSER_H
#define	JSONPARSER_H

#include "JSONType.h"
#include "JSONTokens.h"

#include <sstream>
#include <string>

namespace JSON {

    class SyntaxException : public JSONException {
    private:
        int line_;
        int column_;
    public:
        SyntaxException(std::string msg, int line, int column);

        int line() const;

        int column() const;
    };

    class ParseException {
    private:
        std::string message_;
    public:

        ParseException(std::string message) : message_(message) {
        };

        std::string message() const {
            return message_;
        };
    };

    template<typename JSONTraits, typename Range, typename Listener> class Parser {
    private:
        using Char = typename JSONTraits::Char;
        using CharTraits = typename JSONTraits::CharTraits;
        using String = typename JSONTraits::String;
        using Number = typename JSONTraits::Number;
        using Boolean = typename JSONTraits::Boolean;
        using Iterator = typename Range::Iterator;

        using OutputBuffer = std::basic_ostringstream<Char, CharTraits>;
        using Buffer = std::basic_stringstream<Char, CharTraits>;
        Range current_;
        Listener &listener_;

        Char next() {
            ++current_;
            if (current_) {
                return *current_;
            } else {
                throw ParseException("unexpected end of input");
            }
        };

        bool skipWhitespace() {
            while (current_ && Tokens::whitespace(*current_)) {
                ++current_;
            }
            return current_;
        };

        void skipWhitespaceAndNext() {
            if (!skipWhitespace()) {
                throw ParseException("unexpected end of input");
            }
        };

        int unescapeUnicode() {
            Buffer buffer;
            buffer << std::hex;
            for (int i = 0; i < 4; ++i) {
                int c = next();
                if (!Tokens::hexNumber(c)) {
                    throw ParseException("invalid unicode escape");
                }
                buffer.put(c);
            }
            int result;
            buffer >> result;
            return result;
        };

        String parseString() {
            OutputBuffer buffer;
            int c = next();
            while (!CharTraits::eq(c, Tokens::STRING_DELIMITER)) {
                if (CharTraits::eq(c, Tokens::ESCAPE)) {
                    c = next();
                    if (CharTraits::eq(c, Tokens::UNICODE_ESCAPE)) {
                        c = unescapeUnicode();
                    } else {
                        c = Tokens::unescape(c);
                        if (!c) {
                            throw ParseException("invalid escape character");
                        }
                    }
                } else {
                    buffer.put(c);
                }
            }
            ++current_;
            return buffer.str();
        };

        bool testLiteral(const int *literal, std::size_t length) {
            Range buffer = current_;
            for (std::size_t i = 0; i < length; ++i, ++literal, ++buffer) {
                if (!(buffer && CharTraits::eq(*buffer, *literal))) {
                    return false;
                }
            }
            current_ = buffer;
            return true;
        }

        Number parseNumber() {
            Buffer buffer;
            while (current_ && JSON::Tokens::number(*current_)) {
                buffer.put(*current_);
                ++current_;
            }
            Number result;
            buffer >> result;
            if (buffer.bad()) {
                throw ParseException("invalid number literal");
            }
            return result;
        };

        void parseNumberOrLiteral() {
            if (testLiteral(JSON::Tokens::LITERAL_TRUE, JSON::Tokens::LITERAL_TRUE_LENGTH)) {
                listener_.boolean(true);
            } else if (testLiteral(JSON::Tokens::LITERAL_FALSE, JSON::Tokens::LITERAL_FALSE_LENGTH)) {
                listener_.boolean(false);
            } else if (testLiteral(JSON::Tokens::LITERAL_NULL, JSON::Tokens::LITERAL_NULL_LENGTH)) {
                listener_.null();
            } else {
                listener_.number(parseNumber());
            }
        };

        void parseArray() {
            listener_.arrayBegin();
            next();
            skipWhitespaceAndNext();
            if (CharTraits::eq(*current_, Tokens::ARRAY_END)) {
                ++current_;
                listener_.arrayEnd();
            } else {
                while (true) {
                    parseBranch();
                    skipWhitespaceAndNext();
                    int c = *current_;
                    if (CharTraits::eq(c, Tokens::ELEMENT_SEPARATOR)) {
                        next();
                        skipWhitespaceAndNext();
                    } else if (CharTraits::eq(c, Tokens::ARRAY_END)) {
                        ++current_;
                        listener_.arrayEnd();
                        return;
                    } else {
                        throw ParseException("unexpected token: expected element separator or array end");
                    }
                }
            }
        };

        void parseFieldName() {
            skipWhitespaceAndNext();
            listener_.field(parseString());
            skipWhitespaceAndNext();
            if (*current_ == Tokens::KEY_VALUE_SEPARATOR) {
                ++current_;
            }
        };

        void parseObject() {
            listener_.objectBegin();
            next();
            skipWhitespaceAndNext();
            if (CharTraits::eq(*current_, Tokens::OBJECT_END)) {
                ++current_;
                listener_.objectEnd();
            } else {
                while (true) {
                    parseFieldName();
                    parseBranch();
                    skipWhitespaceAndNext();
                    int c = *current_;
                    if (CharTraits::eq(c, Tokens::ELEMENT_SEPARATOR)) {
                        next();
                        skipWhitespaceAndNext();
                    } else if (CharTraits::eq(c, Tokens::ARRAY_END)) {
                        ++current_;
                        listener_.objectEnd();
                        return;
                    } else {
                        throw ParseException("unexpected token: expected element separator or array end");
                    }
                }
            }
        };

        void parseBranch() {
            skipWhitespaceAndNext();
            int c = *current_;
            if(c == Tokens::ARRAY_BEGIN){
                parseArray();
            }else if(c == Tokens::OBJECT_BEGIN){
                parseObject();
            }else if(c == Tokens::STRING_DELIMITER){
                parseString();
            }else{
                parseNumberOrLiteral();
            }
        };

    public:

        Parser(Range range, Listener &listener) : current_(range), listener_(listener) {
        };

        ~Parser() {
        };

        void parse() {
            try {
                parseBranch();
                skipWhitespace();
                if (current_) {
                    throw ParseException("");
                }
            } catch (ParseException &e) {
                listener_.error(e.message());
            }
        };
        
        Range current() const{
            return current_;
        };
    };

}

#endif	/* JSONPARSER_H */

