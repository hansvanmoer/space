/* 
 * File:   StringBundle.h
 * Author: hans
 *
 * Created on 24 January 2015, 17:18
 */

#ifndef STRINGBUNDLE_H
#define	STRINGBUNDLE_H

#include "Resource.h"
#include "Path.h"
#include "Unicode.h"

#include <locale>

namespace Core{
    
    class StringBundle : public ResourceBundle<std::string, Unicode::String>{
    public:
        StringBundle();
        ~StringBundle();
        
        void load(std::istream &input);
        
    private:

        void parse(const char32_t *buffer, std::streamsize length);
        
        const char32_t *parseStatement(const char32_t *begin, const char32_t *end, int line);
        
        StringBundle(const StringBundle &) = delete;
        StringBundle &operator=(const StringBundle &) = delete;
    };

}

#endif	/* STRINGBUNDLE_H */

