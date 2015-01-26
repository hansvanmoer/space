#include "StringBundle.h"

#include "String.h"

using namespace Core;


StringBundle::StringBundle(){}

Unicode::Character *StringBundle::parseStatement(Unicode::Character *current, Unicode::Character *end, int line){
    Unicode::Character *keyBegin{current};
    Unicode::Character *keyEnd{};
    while(current != end && (!keyEnd)){
        Unicode::CharacterTraits::int_type c = Unicode::CharacterTraits::to_int_type(*current);
        std::cout << std::hex << c << std::dec<<std::endl;
        if(c > 127){
            throw ResourceException(toString("invalid character in key at line ",line));
        }else if(Unicode::CharacterTraits::eq_int_type(c, '=')){
            keyEnd = current;
        }else if(Unicode::CharacterTraits::eq_int_type(c, '\n')){
            throw ResourceException(toString("unexpected newline in key at line ",line));
        }
        ++current;
    }
    if(keyEnd && current != end){
        while(current != end && (!Unicode::CharacterTraits::eq(*current, '\n'))){
            ++current;
        }
        ResourceBundle<std::string, Unicode::String>::add(std::string{keyBegin, keyEnd}, new Unicode::String{keyEnd+1, current});
        if(current != end){
            ++current;
        }
        return current;
    }else{
        throw ResourceException{toString("unexpected end of input at line ",line)};
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

void StringBundle::load(Unicode::InputStream &input){
    if(input.bad()){
        throw ResourceException("unable to read resources from stream");
    }
    std::streampos cur{input.tellg()};
    input.seekg(0, std::ios::end);
    std::streamsize length{input.tellg() - cur};
    input.seekg(cur);
    if(length > 0){
        Unicode::Character *buffer = new Unicode::Character[length];
        input.read(buffer, length);
        try{
            if(true){//TODO:input.eof()){
                parse(buffer, length);
                delete[] buffer;
            }else{
                throw ResourceException("an error occurred reading resource from stream ");
            }
        }catch(ResourceException &e){
            delete[] buffer;
            throw;
        }
    }
}

StringBundle::~StringBundle(){}


