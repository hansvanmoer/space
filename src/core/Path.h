/* 
 * File:   Path.h
 * Author: hans
 *
 * Created on 18 January 2015, 13:36
 */

#ifndef PATH_H
#define	PATH_H

#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

namespace Core{
    
    class Path{
    public:
        Path();
        Path(std::string data);
        Path(Path parent, std::string data);
        
        Path parent() const;
        
        Path child(std::string name) const;
        
        bool fileExists() const;
        
        bool folderExists() const;
        
        bool createFile() const;
        
        bool createFolder() const;
        
        std::string data() const;
        
        template<typename Char, typename CharTraits> bool openFile(std::basic_ofstream<Char,CharTraits> &output) const{
            output.open(data_.c_str());
            return output.good();
        };
        
        template<typename Char, typename CharTraits>  bool openFile(std::basic_ifstream<Char,CharTraits> &input) const{
            input.open(data_.c_str());
            return input.good();
        };

    private:
        std::string data_;
        
        static Path applicationPath_;
    };
    
    class PathException : public std::runtime_error{
    public:
        PathException(std::string message);
    };
}

std::ostream &operator<<(std::ostream &output, const Core::Path &path);

#endif	/* PATH_H */

