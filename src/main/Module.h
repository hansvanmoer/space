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
#include <set>

namespace Game{
    
    class ModuleException : public std::runtime_error{
    public:
        ModuleException(std::string msg);
    };
    
    struct ModuleDescriptor{
        std::string moduleId;
        Core::Path path;
        std::list<std::string> requiredModuleIds;
        std::list<std::string> languageIds;
        
        ModuleDescriptor();
    };
    
    struct LanguageDescriptor{
        std::string id;
        std::string parentId;
        std::string name;
        std::string localeName;
        
        LanguageDescriptor();
        
    };
    
    bool operator<(const LanguageDescriptor &first, const LanguageDescriptor &second);
    
    class Module{
    public:    
        
        Module(const ModuleDescriptor *descriptor, const std::list<const ModuleDescriptor *> &dependencies, const std::list<const Core::Language *> &languages);
                
        ~Module();
        
        std::string id() const;
        
        Core::Path path() const;
        
        std::list<Core::Path> paths() const;
                
        const std::list<const Core::Language *> &languages() const;
        
    private:
        const ModuleDescriptor *descriptor_;
        std::list<const ModuleDescriptor *> dependencies_;
        std::list<const Core::Language *> languages_;
        
        Module(const Module &) = delete;
        Module &operator=(const Module &) = delete;
    };
    
    class ModuleLoader{
    public:
        
        static const std::string NAME;
        
        ModuleLoader();
        
        void addModule(Core::Path modulePath);
        
        void addModules(Core::Path modulesFolder);
        
        const Module *loadModule(std::string moduleId);
        
        const Module *activeModule() const;
        
        ~ModuleLoader();
    private:
        
        const Module *activeModule_;
        
        std::map<std::string, const ModuleDescriptor *> modules_;
        
        void readModuleDescriptor(Core::Path modulePath, ModuleDescriptor &descriptor) const;
        
        std::list<const ModuleDescriptor *> dependencies(std::string moduleId) const;
                
        void dependencies(std::string moduleId, std::list<const ModuleDescriptor *> &dependencies) const;
        
        std::locale createLocale(std::string name) const;
        
        void readLanguageDescriptors(Core::Path languagePath, std::set<LanguageDescriptor> &descriptors) const;
        
        void createLanguages(std::set<LanguageDescriptor> descriptors, const Core::StringBundle &labels, std::set<const Core::Language *> &languages) const;
        
        ModuleLoader(const ModuleLoader &) = delete;
        ModuleLoader &operator=(const ModuleLoader &) = delete;
    };
    
}

#endif	/* APPLICATIONLANGUAGE_H */