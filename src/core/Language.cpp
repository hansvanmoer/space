#include "Language.h"
#include "Path.h"

using namespace Core;

Language::Language(const Language *parent, std::string id, UnicodeString name, const std::locale& locale) : parent_(parent), id_(id), name_(name), locale_(locale){
}

const std::locale& Language::locale() const {
    return locale_;
}

const UnicodeString& Language::name() const {
    return name_;
}

const std::string& Language::id() const {
    return id_;
}

const Language* Language::parent() const {
    return parent_;
}

