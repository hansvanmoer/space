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

        template<typename Arg> void write(std::ostream &buffer, Arg arg) {
            buffer << arg;
        };

        template<typename Arg, typename... Args> void write(std::ostream &buffer, Arg arg, Args... args) {
            buffer << arg;
            write < Args...>(buffer, args...);
        }
    }

    template<typename... Args> std::string toString(Args... args) {
        std::ostringstream buffer;
        String::write(buffer, args...);
        return buffer.str();
    };


}


#endif	/* STRING_H */

