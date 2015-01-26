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

ModuleException::ModuleException(std::string message) : std::runtime_error(message) {
}

struct LanguageData {
    std::string parentId;
    std::string name;
    const LanguageData *parent;
    std::string localeName;
};

const LanguageData *createLanguageData(Object object) {
    LanguageData *data = new LanguageData{};
    try {
        data->parentId = object.findString("parent", "");
        data->name = object.getString("name");
        if (object.hasObject("locale")) {
            Object localeObject = object.getObject("locale");
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
        Document document{JSON::BufferedInput<>
            {input}};
        Array root{document.rootNode().array()};
        for (auto i = root.begin(); i != root.end(); ++i) {
            Object object = (*i).object();
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

Module::Module(Path path) : path_(path) {
    std::ifstream input;
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
    if(found == languages_.end()){
        return nullptr;
    }else{
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

void loadFromPath(Core::StringBundle &bundle, Path path){
    try{
        std::ifstream input;
        path.openFile(input);
        bundle.load(input);
    }catch(Core::ResourceException &e){
        std::cout << "unable to load string bundle from file " << path << ":" << e.what() << std::endl;
    }
};

void Module::load(Core::StringBundle& bundle, Path baseName, const Core::Language *language) const {
    if(language){
        std::list<const Core::Language *> languages;
        const Language *parent = language;
        while(parent){
            languages.push_front(parent);
            parent = parent->parent();
        }
        loadFromPath(bundle, Path{path_, baseName.data()});
        for(auto i = languages.begin(); i != languages.end(); ++i){
            const Language *lang = *i;
            loadFromPath(bundle, Path{path_, baseName.data() + std::string{"_"}+lang->id()});
        }
    }
}

Module::~Module() {
    for (auto i = languages_.begin(); i != languages_.end(); ++i) {
        delete i->second;
    }
}
