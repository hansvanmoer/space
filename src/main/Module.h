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
        ModuleException(std::string msg);
    };
    
    struct ModuleDescriptor{
        std::string moduleId;
        Core::Path path;
        std::list<std::string> requiredModuleIds;
        std::list<std::string> languageIds;
        
        ModuleDescriptor();
    };
    
    class Module{
    public:    
        
        Module(const ModuleDescriptor *descriptor, const std::list<const ModuleDescriptor *> &dependencies);
        
        std::string id() const;
        
        Core::Path path() const;
        
        std::list<Core::Path> paths() const;
                
    private:
        const ModuleDescriptor *descriptor_;
        std::list<const ModuleDescriptor *> dependencies_;
    };
    
    class ModuleLoader{
    public:
        ModuleLoader();
        
        
        void addModule(Core::Path modulePath);
        
        void addModules(Core::Path modulesFolder);
        
        Module loadModule(std::string moduleId) const;
        
        ~ModuleLoader();
    private:
        std::map<std::string, const ModuleDescriptor *> modules_;
        
        void readModuleDescriptor(Core::Path modulePath, ModuleDescriptor &descriptor);
        
        std::list<const ModuleDescriptor *> dependencies(std::string moduleId) const;
                
        void dependencies(std::string moduleId, std::list<const ModuleDescriptor *> &dependencies) const;
        
        ModuleLoader(const ModuleLoader &) = delete;
        ModuleLoader &operator=(const ModuleLoader &) = delete;
    };
    
}

#endif	/* APPLICATIONLANGUAGE_H */