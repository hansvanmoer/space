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
#include <locale>

namespace Core {

    namespace Unicode {
        typedef char32_t Character;
        typedef std::char_traits<char32_t> CharacterTraits;
        typedef std::basic_string<Character> String;
        typedef std::basic_istream<char32_t> InputStream;
        typedef std::basic_ifstream<char32_t> FileInputStream;
        typedef std::basic_ofstream<char32_t> FileOutputStream;
        typedef std::basic_ostringstream<char32_t> StringInputStream;
        typedef std::basic_ostringstream<char32_t> StringOutputStream;
    }

    typedef Unicode::String UnicodeString;

    template<typename Char, typename CharTraits> std::basic_string<Char, CharTraits> convertString(const UnicodeString &string) {
        std::basic_ostringstream<Char, CharTraits> buffer;
        for (auto i = string.begin(); i != string.end(); ++i) {
            buffer.put(*i);
        }
        return buffer.str();
    };

    template<typename Char, typename CharTraits> std::basic_string<Char, CharTraits> convertString(UnicodeString &&string) {
        std::basic_ostringstream<Char, CharTraits> buffer;
        for (auto i = string.begin(); i != string.end(); ++i) {
            buffer.put(*i);
        }
        return buffer.str();
    };

}


#endif	/* UNICODE_H */

