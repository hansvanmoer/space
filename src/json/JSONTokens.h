/* 
 * File:   JSONTokens.h
 * Author: hans
 *
 * Created on 30 December 2014, 13:34
 */

#ifndef JSONTOKENS_H
#define	JSONTOKENS_H

#include <string>

namespace JSON{
    
    namespace Tokens {

        extern const int OBJECT_BEGIN;

        extern const int OBJECT_END;

        extern const int ARRAY_BEGIN;

        extern const int ARRAY_END;

        extern const int ELEMENT_SEPARATOR;

        extern const int KEY_VALUE_SEPARATOR;

        extern const int STRING_DELIMITER;

        extern const int ESCAPE;

        extern const int UNICODE_ESCAPE;

        extern const int CARRIAGE_RETURN;

        extern const int FORM_FEED;

        extern const int LINE_FEED;

        extern const int QUOTE;

        extern const int SOLIDUS;

        extern const int BACKSPACE;

        extern const int TAB;

        extern const int REVERSE_SOLIDUS;

        extern const int LITERAL_TRUE[4];
        
        extern const std::size_t LITERAL_TRUE_LENGTH;

        extern const int LITERAL_FALSE[5];
        
        extern const std::size_t LITERAL_FALSE_LENGTH;

        extern const int LITERAL_NULL[4];
        
        extern const std::size_t LITERAL_NULL_LENGTH;

        bool whitespace(int code);

        int unescape(int code);

        int escape(int code);

        bool number(int code);
        
        bool hexNumber(int code);
        
        bool invalidInString(int code);
    }
    
}

#endif	/* JSONTOKENS_H */

