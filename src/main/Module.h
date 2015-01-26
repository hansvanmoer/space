/* 
 * File:   ApplicationLanguage.h
 * Author: hans
 *
 * Created on 24 January 2015, 13:46
 */

#ifndef MODULE_H
#define	MODULE_H

#include "Language.h"
#include "Path.h"
#include "StringBundle.h"

#include <string>
#include <map>
#include <stdexcept>

namespace Game{
    
    class ModuleException : public std::runtime_error{
    public:
        ModuleException(std::string message);
    };
    
    class Module{
    public:
        
        using LanguageMap = std::map<std::string, const Core::Language *>;
        
        Module(Core::Path path);

        ~Module();
        
        Core::Path path();
        
        const std::string &id() const;
        
        bool loadDefault() const;
        
        const Core::Language *language(std::string id) const;

        const LanguageMap &languages() const;
        
        void load(Core::StringBundle &bundle, Core::Path basePath, const Core::Language *language) const;
        
    private:
        Core::Path path_;
        std::string id_;
        bool loadDefault_;
        std::map<std::string, const Core::Language *> languages_;
        Core::StringBundle labels_;
    };
    
}

#endif	/* APPLICATIONLANGUAGE_H */