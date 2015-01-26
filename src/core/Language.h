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
    public:
        Language(const Language *parent, const std::string id, UnicodeString name, const std::locale &locale);
        
        const Language *parent() const;
        
        const std::string &id() const;
        
        const UnicodeString &name() const;
        
        const std::locale &locale() const;
    
    private:
        const Language *parent_;
        std::string id_;
        UnicodeString name_;
        const std::locale locale_;
    
        Language(const Language &) = delete;
        Language &operator=(const Language &) = delete;
    };
    

}

#endif	/* LANGUAGE_H */

