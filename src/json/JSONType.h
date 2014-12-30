/* 
 * File:   JSON.h
 * Author: hans
 *
 * Created on 30 December 2014, 13:19
 */

#ifndef JSON_TYPE_H
#define	JSON_TYPE_H

#include <stdexcept>
#include <string>
#include <iostream>

namespace JSON{

    template<typename Char_, typename CharTraits_, typename CharAllocator_> class BasicJSONTraits{
    public:
        using Char = Char_;
        using CharTraits = CharTraits_;
        using CharAllocator = CharAllocator_;
        using String = std::basic_string<Char,CharTraits,CharAllocator>;
        using Number = double;
        using Boolean = bool;
        
        void print(std::ostream &output, String string){
            for(auto i = string.begin(); i != string.end(); ++i){
                output.put(CharTraits::to_int_type(i));
            }
        };
    };
    
    template<> class BasicJSONTraits<char, std::char_traits<char>, std::allocator<char> >{
    public:
        using Char = char;
        using CharTraits = std::char_traits<char>;
        using CharAllocator = std::allocator<char>;
        using String = std::string;
        using Number = double;
        using Boolean = bool;
        
        void print(std::ostream &output, String string){
            output << string;
        };
    };
    
    enum class NodeType{
        NULL_VALUE, OBJECT, ARRAY, STRING, NUMBER, BOOLEAN 
    };
    
    std::ostream &operator<<(std::ostream &output, const JSON::NodeType &type);
    
    class JSONException : public std::runtime_error{
    public:
        JSONException(std::string msg);
    };
    
}

#endif	/* JSON_H */

