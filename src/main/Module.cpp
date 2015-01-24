#include "Module.h"

#include "JSONReader.h"
#include "System.h"
#include "Application.h"
#include "String.h"

#include <list>

using namespace Game;

using Core::Path;
using Core::Language;

using Document = JSON::Document<JSON::BufferedInput<> >;
using Object = typename JSON::Document<JSON::BufferedInput<> >::Object;
using Array = typename JSON::Document<JSON::BufferedInput<> >::Array;
using ArrayIterator = typename Array::Iterator;

ModuleException::ModuleException(std::string message) : std::runtime_error(message){}

const LanguageData *createLanguageData(Object object){
    LanguageData *data = new LanguageData{};
    try{
        data->parentId=object.findString("parent","");
        if(object.hasObject("locale")){
#ifdef OS_UNIX_LIKE
            data->localeName = object.getObject("locale").getString("posix");
#endif
#ifdef OS_WINDOWS
            data->localeName = object.getObject("locale").getString("windows");
#endif
        }
        return data;
    }catch(JSON::JSONException &e){
        delete data;
        throw;
    }
};

Core::UnicodeString createName(const std::string &localeId){
    return Core::UnicodeString{};
};

std::locale createLocale(const LanguageData &data){
    return std::locale{data.localeName.data()};
};

std::map<std::string, const Language *> createLanguageTree(std::map<std::string, const LanguageData *> &input){
    std::map<std::string, const Language *> result;
    while(!input.empty()){
        std::list<std::string> done; 
        for(auto i = input.begin(); i != input.end(); ++i){
            const LanguageData *data = i->second;
            if(data->parentId.empty()){
                result.insert(std::make_pair(i->first, new Language{nullptr, i->first, createName(i->first), createLocale(*data)}));
                done.push_back(i->first);
            }else{
                auto parent = result.find(data->parentId);
                if(parent != result.end()){
                    result.insert(std::make_pair(i->first, new Language{parent->second, i->first, createName(i->first), createLocale(*data)}));
                    done.push_back(i->first);
                }
            }
        }
        if(done.empty()){
            for(auto i = result.begin(); i != result.end(); ++i){
                delete i->second;
            }
            std::ostringstream buffer;
            buffer << "module was not able to create language tree." << std::endl << "possible erroneous languages:" << std::endl;
            for(auto i = input.begin(); i != input.end(); ++i){
                buffer << i->first << std::endl;
            }
            throw ModuleException{buffer.str()};
        }else{
            for(auto i = done.begin(); i != done.end(); ++i){
                auto found = input.find(*i);
                delete found->second;
                input.erase(found);
            }
        }
    }
    return result;
};

std::map<std::string, const Language *> createLanguages(Path path, std::list<std::string> available){
    std::ifstream input;
    Path languageData{path, "language"};
    languageData.openFile(input);
    std::map<std::string, const LanguageData *> languages;
    try{
        Document document{JSON::BufferedInput<>{input}};
        Array root{document.rootNode().array()};
        for(auto i = root.begin(); i != root.end(); ++i){
            Object object = (*i).object();
            languages.insert(std::make_pair(object.getString("id"),createLanguageData(object)));
        }
        return createLanguageTree(languages);
    }catch(std::exception &e){
        for(auto i = languages.begin(); i != languages.end(); ++i){
            delete i->second;
        }
        throw ModuleException{Core::toString("unable to parse languages: ",e.what())};
    }
};

Module::Module(Path path) : path_(path){
    std::ifstream input;
    Path moduleData{path_, "module"};
    moduleData.openFile(input);
    try{
        Document document{JSON::BufferedInput<>{input}};
        Object root{document.rootNode().object()};
        id_ = root.getString("id");
        loadDefault_ = root.getBoolean("loadDefault");
        Array languages{root.getArray("languages")};
        std::list<std::string> availableLanguageCodes;
        for(auto i = languages.begin(); i != languages.end(); ++i){
            availableLanguageCodes.push_back((*i).string());
        }
        languages_=createLanguages(path, availableLanguageCodes);
    }catch(JSON::JSONException &e){
        throw ModuleException(Core::toString("unable to load module data due to JSON error: ",e.what()));
    }
}

const Module::LanguageMap & Module::languages() const {
    return languages_;
}


Module::~Module(){
    for(auto i = languages_.begin(); i != languages_.end(); ++i){
        delete i->second;
    }
}
