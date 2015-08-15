/* 
 * File:   String.h
 * Author: hans
 *
 * Created on 24 January 2015, 15:28
 */

#ifndef STRING_H
#define	STRING_H

#include <iostream>
#include <sstream>
#include <string>
#include <locale>

namespace Core {


    namespace String {

        template<typename T> class WritePolicy {
        public:

            static void execute(std::ostream &output, const T &data) {
                output << data;
            };
        };

        template<typename Char, typename Traits, typename Allocator> class WritePolicy<std::basic_string<Char, Traits, Allocator>>
        {
            public:

            static void execute(std::ostream &output, std::basic_string<Char, Traits, Allocator> &data) {
                for(auto c : data){
                    typename Traits::int_type i = Traits::to_int_type(c);
                    output.put((i >= 0 && i < 128) ? i : '?');
                }
            };
        };

        template<typename Allocator> class WritePolicy<std::basic_string<char, std::char_traits<char>, Allocator>>
        {
            public:

            static void execute(std::ostream &output, std::basic_string<char, std::char_traits<char>, Allocator> data) {
                output << data;
            };
        };

        template<typename Arg> void write(std::ostream &buffer, Arg arg) {
            WritePolicy<Arg>::execute(buffer, arg);
        };

        template<typename Arg, typename... Args> void write(std::ostream &buffer, Arg &&arg, Args &&... args) {
            WritePolicy<Arg>::execute(buffer, arg);
            write < Args...>(buffer, args...);
        }
    }

    template<typename... Args> std::string toString(Args &&... args) {
        std::ostringstream buffer;
        String::write(buffer, args...);
        return buffer.str();
    };


}


#endif	/* STRING_H */

