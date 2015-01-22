/* 
 * File:   Path.h
 * Author: hans
 *
 * Created on 18 January 2015, 13:36
 */

#ifndef PATH_H
#define	PATH_H

#include <string>
#include <fstream>
#include <stdexcept>

namespace Game{
    
    class Path{
    public:
        Path();
        Path(std::string data);
        Path(Path parent, std::string data);
        
        Path child(std::string name);
        
        bool fileExists() const;
        
        bool folderExists() const;
        
        bool createFile() const;
        
        bool createFolder() const;
        
        std::string data() const;
        
        bool openFile(std::ofstream &output) const;
        
        bool openFile(std::ifstream &input) const;

    private:
        std::string data_;
        
        static Path applicationPath_;
    };
    
    
    class PathException : public std::runtime_error{
    public:
        PathException(std::string message);
    };
    
}

#endif	/* PATH_H */

