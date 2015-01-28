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

ModuleException::ModuleException(std::string msg) : std::runtime_error{msg}{
}

ModuleDescriptor::ModuleDescriptor() {}

void ModuleLoader::readModuleDescriptor(Path modulePath, ModuleDescriptor &descriptor){
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
            throw ModuleException{Core::toString("unable to parse module descriptor", modulePath, e.what())};
        }
    }else{
        throw ModuleException{Core::toString("unable to read module descriptor", modulePath)};
    }
};

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

Module ModuleLoader::loadModule(std::string moduleId) const{
    auto found = modules_.find(moduleId);
    if(found == modules_.end()){
        throw ModuleException{Core::toString("no module found for id '", moduleId,"'")};
    }else{
        std::list<const ModuleDescriptor *> modules = dependencies(moduleId);
        return Module{found->second, modules};
    }
};

Module::Module(const ModuleDescriptor* descriptor, const std::list<const ModuleDescriptor*>& dependencies) : descriptor_(descriptor), dependencies_(dependencies){};

std::string Module::id() const{
    return descriptor_->moduleId;
}

Core::Path Module::path() const{
    return descriptor_->path;
}

std::list<Core::Path> Module::paths() const{
    std::list<Core::Path> paths;
    for(auto i = dependencies_.begin(); i != dependencies_.end(); ++i){
        paths.push_back((*i)->path);
    }
    return paths;
}


/*

ModuleException::ModuleException(std::string message) : std::runtime_error(message) {
}

struct LanguageData {
    std::string parentId;
    std::string name;
    const LanguageData *parent;
    std::string localeName;
};

const LanguageData *createLanguageData(IO::Object object) {
    LanguageData *data = new LanguageData{};
    try {
        data->parentId = object.findString("parent", "");
        data->name = object.getString("name");
        if (object.hasObject("locale")) {
            IO::Object localeObject = object.getObject("locale");
#ifdef OS_UNIX_LIKE
            localeObject.getString("windows");
            data->localeName = localeObject.getString("posix");
#endif
#ifdef OS_WINDOWS
            localeObject.getString("posix");
            data->localeName = localeObject.getString("windows");
#endif
        }
        return data;
    } catch (JSON::JSONException &e) {
        std::cout << "error while parsing language file: " << e.what() << std::endl;
        delete data;
        throw;
    }
};

Core::UnicodeString createName(const LanguageData &data) {
    return Core::UnicodeString{data.name.begin(), data.name.end()};
};

std::locale createLocale(const LanguageData &data, const Language *parent) {
    if (data.localeName.empty()) {
        return parent->locale();
    } else {
        try {
            std::locale loc{data.localeName.data()};
            return loc;
        } catch (std::exception &e) {
            std::cout << "unable to create locale with name " << data.localeName << std::endl;
            std::cout << "using default locale" << std::endl;
            return std::locale::classic();
        }
    }
};

std::map<std::string, const Language *> createLanguageTree(std::map<std::string, const LanguageData *> &input) {
    std::map<std::string, const Language *> result;
    while (!input.empty()) {
        std::list<std::string> done;
        for (auto i = input.begin(); i != input.end(); ++i) {
            const LanguageData *data = i->second;
            if (data->parentId.empty()) {
                result.insert(std::make_pair(i->first, new Language {
                    nullptr, i->first, createName(*i->second), createLocale(*data, nullptr)
                }));
                done.push_back(i->first);
            } else {
                auto parent = result.find(data->parentId);
                if (parent != result.end()) {
                    result.insert(std::make_pair(i->first, new Language {
                        parent->second, i->first, createName(*i->second), createLocale(*data, parent->second)
                    }));
                    done.push_back(i->first);
                }
            }
        }
        if (done.empty()) {
            for (auto i = result.begin(); i != result.end(); ++i) {
                delete i->second;
            }
            std::ostringstream buffer;
            buffer << "module was not able to create language tree." << std::endl << "possible erroneous languages:" << std::endl;
            for (auto i = input.begin(); i != input.end(); ++i) {
                buffer << i->first << std::endl;
            }
            throw ModuleException{buffer.str()};
        } else {
            for (auto i = done.begin(); i != done.end(); ++i) {
                auto found = input.find(*i);
                delete found->second;
                input.erase(found);
            }
        }
    }
    return result;
};

std::map<std::string, const Language *> createLanguages(Path path, std::list<std::string> available) {
    std::ifstream input;
    Path languageData{path, "language"};
    languageData.openFile(input);
    std::map<std::string, const LanguageData *> languages;
    try {
        IO::Document document{JSON::BufferedInput<>
            {input}};
        IO::Array root{document.rootNode().array()};
        for (auto i = root.begin(); i != root.end(); ++i) {
            IO::Object object = (*i).object();
            languages.insert(std::make_pair(object.getString("id"), createLanguageData(object)));
        }
        return createLanguageTree(languages);
    } catch (std::exception &e) {
        for (auto i = languages.begin(); i != languages.end(); ++i) {
            delete i->second;
        }
        throw ModuleException{Core::toString("unable to parse languages: ", e.what())};
    }
};

struct ModuleData {
    std::string id;
    std::list<std::string> requiredModuleIds;
    std::list<std::string> languageIds;
    
    ModuleData(std::string id_) : id(id_), requiredModuleIds(), languageIds(){};
};

void loadModuleDescriptor(Path modulePath, ModuleData &data, std::set<std::string> &alreadyLoaded) {
    Path file{modulePath.child("module")};
    std::ifstream input;
    if (file.openFile(input)) {
        try {
            IO::Document document{IO::open(input)};
            IO::Object root{document.rootNode().object()};
            IO::Array languageArray{root.getArray("languages")};
            for (auto i = languageArray.begin(); i != languageArray.end(); ++i) {
                data.languageIds.push_back((*i).string());
            }
            if (root.hasArray("requiredModules")) {
                IO::Array requiredModulesArray{root.getArray("requiredModules")};
                for (auto i = requiredModulesArray.begin(); i != requiredModulesArray.end(); ++i) {
                    std::string required = (*i).string();
                    auto found = std::find(data.requiredModuleIds.begin(), data.requiredModuleIds.end(), required);
                    if(found == data.requiredModuleIds.end()){
                        ModuleData requiredData;
                        
                    }
                }
            }
            
        } catch (JSON::JSONException &e) {
            throw ModuleException(Core::toString("unable to parse module descriptor '", file, "': ", e.what()));
        }
    } else {
        throw ModuleException(Core::toString("unable to open module descriptor: '", file, "', check if file exists and is not empty"));
    }
};

Module::Module(Path path) : path_(path) {
    /*std::ifstream input;
    Path moduleData{path_, "module"};
    moduleData.openFile(input);
    try {
        Document document{JSON::BufferedInput<>
            {input}};
        Object root{document.rootNode().object()};
        id_ = root.getString("id");
        loadDefault_ = root.getBoolean("loadDefault");
        Array languages{root.getArray("languages")};
        std::list<std::string> availableLanguageCodes;
        for (auto i = languages.begin(); i != languages.end(); ++i) {
            availableLanguageCodes.push_back((*i).string());
        }
        languages_ = createLanguages(path, availableLanguageCodes);
    } catch (JSON::JSONException &e) {
        throw ModuleException(Core::toString("unable to load module data due to JSON error: ", e.what()));
    }
}

const Module::LanguageMap & Module::languages() const {
    return languages_;
}

const Core::Language* Module::language(std::string id) const {
    auto found = languages_.find(id);
    if (found == languages_.end()) {
        return nullptr;
    } else {
        return found->second;
    }
}

const std::string& Module::id() const {
    return id_;
}

Core::Path Module::path() {
    return path_;
}

bool Module::loadDefault() const {
    return loadDefault_;
}

void loadFromPath(Core::StringBundle &bundle, Path path) {
    try {
        std::ifstream input;
        path.openFile(input);
        bundle.load(input);
    } catch (Core::ResourceException &e) {
        std::cout << "unable to load string bundle from file " << path << ":" << e.what() << std::endl;
    }
};

void Module::load(Core::StringBundle& bundle, Path baseName, const Core::Language *language) const {
    if (language) {
        std::list<const Core::Language *> languages;
        const Language *parent = language;
        while (parent) {
            languages.push_front(parent);
            parent = parent->parent();
        }
        loadFromPath(bundle, Path{path_, baseName.data()});
        for (auto i = languages.begin(); i != languages.end(); ++i) {
            const Language *lang = *i;

            loadFromPath(bundle, Path{path_, baseName.data() + std::string {
                    "_"}
                +lang->id()});
        }
    }
}

Module::~Module() {
    for (auto i = languages_.begin(); i != languages_.end(); ++i) {
        delete i->second;
    }
}
 * 
 * */
