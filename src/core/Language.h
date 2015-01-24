/* 
 * File:   Language.h
 * Author: hans
 *
 * Created on 24 January 2015, 11:33
 */

#ifndef LANGUAGE_H
#define	LANGUAGE_H

#include "Unicode.h"
#include "Resource.h"
#include "Path.h"

#include <locale>
#include <string>
#include <unordered_map>

namespace Core{
    
    class Language{
        
        Language(const Language *parent, const std::string code, UnicodeString name, const std::locale &locale);
        
        const Language *parent() const;
        
        const std::string &code() const;
        
        const UnicodeString &name() const;
        
        const std::locale &locale() const;
    private:
        const Language *parent_;
        std::string code_;
        UnicodeString name_;
        const std::locale locale_;
    
        Language(const Language &) = delete;
        Language &operator=(const Language &) = delete;
    };
    
    class StringBundle : public ResourceBundle<std::string, UnicodeString>{
    public:
        StringBundle();
        ~StringBundle();
        
        void load(Path path);
        
    private:

        void parse(Unicode::Character *buffer, std::streamsize length);
        
        Unicode::Character *parseStatement(Unicode::Character *begin, Unicode::Character *end, int line);
        
        StringBundle(const StringBundle &) = delete;
        StringBundle &operator=(const StringBundle &) = delete;
    };
}

#endif	/* LANGUAGE_H */

