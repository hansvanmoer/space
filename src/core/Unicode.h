/* 
 * File:   Unicode.h
 * Author: hans
 *
 * Created on 24 January 2015, 09:15
 */

#ifndef UNICODE_H
#define	UNICODE_H

#include <string>
#include <sstream>

namespace Core{
    
    typedef std::basic_string<char32_t> UnicodeString;
    
    template<typename Char, typename CharTraits> std::basic_string<Char, CharTraits> convertString(const UnicodeString &string){
        std::basic_ostringstream<Char, CharTraits> buffer;
        for(auto i = string.begin(); i != string.end(); ++i){
            buffer.put(*i);
        }
        return buffer.str();
    };
    
    template<typename Char, typename CharTraits> std::basic_string<Char, CharTraits> convertString(UnicodeString &&string){
        std::basic_ostringstream<Char, CharTraits> buffer;
        for(auto i = string.begin(); i != string.end(); ++i){
            buffer.put(*i);
        }
        return buffer.str();
    };
}


#endif	/* UNICODE_H */

