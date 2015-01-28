#include "Module.h"

#include "JSONReader.h"
#include "System.h"
#include "Application.h"
#include "String.h"
#include "IO.h"

#include <list>
#include <set>
#include <algorithm>

using namespace Game;

using Core::Path;
using Core::Language;
using Core::StringBundle;

ModuleException::ModuleException(std::string msg) : std::runtime_error{msg}{
}

ModuleDescriptor::ModuleDescriptor() {
}

LanguageDescriptor::LanguageDescriptor() : id(), parentId(), name(), localeName(){}

bool Game::operator<(const LanguageDescriptor& first, const LanguageDescriptor &second){
    return first.id < second.id;
};

void ModuleLoader::readModuleDescriptor(Path modulePath, ModuleDescriptor &descriptor) const{
    descriptor.path = modulePath;
    descriptor.moduleId=modulePath.name();
    std::ifstream input;
    Path descriptorPath{modulePath.child("module")};
    descriptorPath.openFile(input);
    if(descriptorPath.fileExists() && input.good()){
        try{
            IO::Document document = IO::open(input);
            IO::Object moduleData = document.rootNode().object();
            if(moduleData.hasArray("requiredModules")){
                IO::Array requiredModulesData = moduleData.getArray("requiredModules");
                std::list<std::string> requiredModuleList;
                for(auto i = requiredModulesData.begin(); i != requiredModulesData.end(); ++i){
                    requiredModuleList.push_back((*i).string());
                }
                descriptor.requiredModuleIds = requiredModuleList;
            }
            IO::Array languageArray = moduleData.getArray("languages");
            std::list<std::string> languages;
            for(auto i = languageArray.begin(); i != languageArray.end(); ++i){
                languages.push_back((*i).string());
            }
            descriptor.languageIds = languages;
        }catch(JSON::JSONException &e){
            throw ModuleException{Core::toString("unable to parse module descriptor: ", modulePath, e.what())};
        }
    }else{
        throw ModuleException{Core::toString("unable to read module descriptor: ", modulePath)};
    }
};

void ModuleLoader::readLanguageDescriptors(Core::Path languagePath, std::set<LanguageDescriptor>& descriptors) const{
    std::ifstream input;
    languagePath.openFile(input);
    if(languagePath.fileExists() && input.good()){
        try{
            IO::Document document = IO::open(input);
            IO::Array array = document.rootNode().array();
            for(auto i = array.begin(); i != array.end(); ++i){
                IO::Object languageData = (*i).object();
                LanguageDescriptor descriptor;
                descriptor.id = languageData.getString("id");
                if(descriptor.id.empty()){
                    throw ModuleException{Core::toString("unable to create language: id must not be empty")};
                }
                descriptor.name = languageData.getString("name");
                if(languageData.hasString("parent")){
                    descriptor.parentId = languageData.getString("parent");
                    if(descriptor.parentId.empty()){
                        throw ModuleException{Core::toString("unable to create language '",descriptor.id,"' locale parent id must not be empty")};
                    }
                }
                if(languageData.hasObject("locale")){
                    IO::Object localeData = languageData.getObject("locale");
#ifdef OS_UNIX_LIKE
                    descriptor.localeName = localeData.getString("posix");
                    localeData.getString("windows");
#endif
#ifdef OS_WINDOWS
                    descriptor.localeName = localeData.getString("windows");
                    localeData.getString("posix");
#endif
                }
                descriptors.insert(descriptor);
            }
        }catch(JSON::JSONException &e){
            throw ModuleException{Core::toString("unable to parse language descriptors from file  ", languagePath, "' : ", e.what())};
        }
    }else{
        throw ModuleException{Core::toString("unable to read language descriptors from file '", languagePath, "' : unable to open file")};
    }
}

std::locale ModuleLoader::createLocale(std::string localeName) const{
    try{
        return std::locale{localeName.c_str()};
    }catch(std::exception &e){
        std::cout << "unable to create locale for name '" << localeName << "' : " << e.what() << std::endl << "creating default locale" << std::endl;
        return std::locale::classic();
    }
};

void ModuleLoader::createLanguages(std::set<LanguageDescriptor> descriptors, const StringBundle &labels, std::set<const Core::Language*>& languages) const{
    std::map<std::string, const Language *> languageMap;
    std::list<LanguageDescriptor> done;
    while(!descriptors.empty()){
        for(auto i = descriptors.begin(); i != descriptors.end(); ++i){
            std::string parentId = i->parentId;
            if(parentId.empty()){
                done.push_back(*i);
                languageMap.insert(std::make_pair(i->id, new Language{nullptr, i->id, *labels[i->id], createLocale(i->localeName)}));
            }else{
                auto parent = languageMap.find(parentId);
                if(parent != languageMap.end()){
                    done.push_back(*i);
                    languageMap.insert(std::make_pair(i->id, new Language{parent->second, i->id, *labels[i->id], createLocale(i->localeName)}));
                }
            }
        }
        if(done.empty()){
            for(auto i = languageMap.begin(); i != languageMap.end(); ++i){
                delete i->second;
            }
            throw ModuleException("unable to create languages: make sure all parent languages are present and no circular references are present");
        }else{
            for(auto i = done.begin(); i != done.end(); ++i){
                descriptors.erase(*i);
            }
            done.clear();
        }
    }
    for(auto i = languageMap.begin(); i != languageMap.end(); ++i){
        languages.insert(i->second);
    }
}

ModuleLoader::ModuleLoader() : modules_(){};

ModuleLoader::~ModuleLoader(){
    for(auto i = modules_.begin(); i != modules_.end(); ++i){
        delete i->second;
    }
}

void ModuleLoader::addModule(Core::Path modulePath){
    ModuleDescriptor *descriptor = new ModuleDescriptor{};
    try{
        readModuleDescriptor(modulePath, *descriptor);
    }catch(std::exception &e){
        delete descriptor;
        throw;
    }
    if(modules_.find(descriptor->moduleId) == modules_.end()){
        modules_.insert(std::make_pair(descriptor->moduleId, descriptor));
    }else{
        delete descriptor;
        throw ModuleException(Core::toString("module '", descriptor->moduleId,"' already loaded"));
    }
}

void ModuleLoader::addModules(Core::Path modulesPath){
    std::list<Path> paths{modulesPath.childFolders()};
    for(auto i = paths.begin(); i != paths.end(); ++i){
        try{
            addModule(*i);
            std::cout << "module descriptor read from path '" << i->data() << "'" << std::endl;
        }catch(std::exception &e){
            std::cout << "unable to add module from path '" << i->data() << "': " << e.what() << std::endl;
        }
    }
}

void ModuleLoader::dependencies(std::string moduleId, std::list<const ModuleDescriptor *> &result) const {
    auto found = modules_.find(moduleId);
    if(found == modules_.end()){
        throw ModuleException{Core::toString("module '", moduleId, "' was not found")};
    }else{
        const ModuleDescriptor *descriptor = found->second;
        if(std::find(result.begin(), result.end(), descriptor) == result.end()){
            result.push_front(descriptor);
            for(auto i = descriptor->requiredModuleIds.begin(); i != descriptor->requiredModuleIds.end(); ++i){
                std::string dependentModuleId = *i;
                dependencies(dependentModuleId, result);
            }
        }
    }
}

std::list<const ModuleDescriptor*> ModuleLoader::dependencies(std::string moduleId) const {
    std::list<const ModuleDescriptor *> result;
    dependencies(moduleId, result);
    return result;
}

bool compareLanguage(const Language *first, const Language *second){
    return first->name() < second->name();
};

Module *ModuleLoader::loadModule(std::string moduleId) const{
    auto found = modules_.find(moduleId);
    if(found == modules_.end()){
        throw ModuleException{Core::toString("no module found for id '", moduleId,"'")};
    }else{
        std::list<const ModuleDescriptor *> modules = dependencies(moduleId);
        std::set<LanguageDescriptor> languageDescriptors;
        Core::StringBundle languageLabels;
        for(auto i = modules.begin(); i != modules.end(); ++i){
            Path modulePath = (*i)->path;
            Path labelPath = modulePath.child("language_labels");
            if(labelPath.fileExists()){
                std::ifstream input;
                labelPath.openFile(input);
                try{
                    languageLabels.load(input);
                }catch(Core::ResourceException &e){
                    std::cout << "unable to load language labels from path '" << labelPath << "' : " << e.what() << std::endl;
                    std::cout << "skipping file" << std::endl;
                }
            }
            try{
                readLanguageDescriptors(modulePath.child("language"), languageDescriptors);
            }catch(ModuleException &e){
                    std::cout << "unable to load language from path '" << labelPath << "' : " << e.what() << std::endl;
                    std::cout << "skipping file" <<std::endl;
            }
        }
        std::set<const Language *> languages;
        createLanguages(languageDescriptors, languageLabels, languages);
        std::list<const Language *> languageList;
        for(auto i = languages.begin(); i != languages.end(); ++i){
            languageList.push_back(*i);
        }
        languageList.sort(compareLanguage);
        return new Module{found->second, modules, languageList};
    }
};

Module::Module(const ModuleDescriptor* descriptor, const std::list<const ModuleDescriptor*>& dependencies, const std::list<const Language *> &languages) : descriptor_(descriptor), dependencies_(dependencies), languages_(languages){};

Module::~Module(){
    for(auto i = languages_.begin(); i != languages_.end(); ++i){
        delete *i;
    }
};

std::string Module::id() const{
    return descriptor_->moduleId;
}

Core::Path Module::path() const{
    return descriptor_->path;
}

const std::list<const Language *> &Module::languages() const{
    return languages_;
};

std::list<Core::Path> Module::paths() const{
    std::list<Core::Path> paths;
    for(auto i = dependencies_.begin(); i != dependencies_.end(); ++i){
        paths.push_back((*i)->path);
    }
    return paths;
}