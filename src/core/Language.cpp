#include "Language.h"
#include "Path.h"

using namespace Core;

Language::Language(const Language *parent, std::string code, UnicodeString name, const std::locale& locale) : parent_(parent), code_(code), name_(name), locale_(locale){
}

const std::locale& Language::locale() const {
    return locale_;
}

const UnicodeString& Language::name() const {
    return name_;
}

const std::string& Language::code() const {
    return code_;
}

const Language* Language::parent() const {
    return parent_;
}

StringBundle::StringBundle(){}

Unicode::Character *StringBundle::parseStatement(Unicode::Character *current, Unicode::Character *end, int line){
    Unicode::Character *keyBegin{current};
    Unicode::Character *keyEnd{};
    while(current != end && (!keyEnd)){
        Unicode::CharacterTraits::int_type c = Unicode::CharacterTraits::to_int_type(*current);
        if(c > 127){
            throw ResourceException(std::string("invalid character in key at line ")+std::to_string(line));
        }else if(Unicode::CharacterTraits::eq_int_type(c, '=')){
            keyEnd = current;
            ++current;
        }else if(Unicode::CharacterTraits::eq_int_type(c, '\n')){
            throw ResourceException(std::string("unexpected newline in key at line ")+std::to_string(line));
        }
    }
    if(keyEnd && current != end){
        while(current != end && Unicode::CharacterTraits::eq(*current, '\n')){
            ++current;
        }
        add(std::string{keyBegin, keyEnd}, new UnicodeString{keyBegin+2, current});
    }else{
        throw ResourceException{std::string("unexpected end of input at line ")+std::to_string(line)};
    }
}

void StringBundle::parse(Unicode::Character *buffer, std::streamsize length){
    Unicode::Character *end = buffer+length;
    int line{0};
    while(buffer != end){
        buffer = parseStatement(buffer, end, line);
        ++line;
    }
}

void StringBundle::load(Path path){
    Core::Unicode::FileInputStream input;
    path.openFile(input);
    if(input.bad()){
        throw ResourceException(std::string{"unable to read resources from file "} + path.data());
    }
    std::streampos cur{input.tellg()};
    input.seekg(0, std::ios::end);
    std::streamsize length{input.tellg() - cur};
    input.seekg(cur);
    if(length > 0){
        Unicode::Character *buffer = new Unicode::Character[length];
        input.read(buffer, length);
        try{
            if(!input.bad()){
                parse(buffer, length);
                delete[] buffer;
            }else{
                throw ResourceException(std::string{"an error occurred reading resource from file "}+path.data());
            }
        }catch(ResourceException &e){
            delete[] buffer;
            throw;
        }
    }
}

StringBundle::~StringBundle(){}


