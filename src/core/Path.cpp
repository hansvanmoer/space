#include "Path.h"
#include "System.h"

#include <iostream>
#include <sstream>

#ifdef OS_UNIX_LIKE
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#endif

using namespace Core;

#ifdef OS_UNIX_LIKE

const char SEPARATOR = '/';

#endif
#ifdef OS_WINDOWS
const char SEPARATOR = '\\';
#endif

std::string concatenate(const std::string& parent, const std::string& child) {
    std::ostringstream buffer;
    buffer << parent;
    buffer.put(SEPARATOR);
    buffer << child;
    return buffer.str();
}

Path::Path() : data_() {
};

Path::Path(std::string data) : data_(data) {
};

Path::Path(Path parent, std::string name) : data_(concatenate(parent.data_, name)) {
};

Path Path::parent() const{
    std::string::size_type pos = data_.find_last_of(SEPARATOR);
    if(pos == std::string::npos){
        return Path{};
    }else{
        return Path{data_.substr(0,pos)};
    }
};

std::string Path::name() const{
    std::string::size_type pos = data_.find_last_of(SEPARATOR);
    if(pos == std::string::npos){
        return data_;
    }else if(pos == (data_.length() - 1)){
        return std::string{};
    }else{
        return data_.substr(pos+1, data_.length());
    }
};

Path Path::child(std::string name) const{
    return Path{data_, name};
}

bool Path::fileExists() const {
#ifdef OS_UNIX_LIKE
    struct stat buffer;
    return (stat(data_.c_str(), &buffer) == 0);
#endif
#ifdef OS_WINDOWS
    //TODO
#endif
}

bool Path::folderExists() const {
#ifdef OS_UNIX_LIKE
    DIR* dir = opendir(data_.c_str());
    if (dir) {
        closedir(dir);
        return true;
    } else if (ENOENT == errno) {
        return false;
    } else {
        //this is bad...
        throw PathException("unable to check if folder exists");
    }
#endif
#ifdef OS_WINDOWS
    //TODO
#endif
}

bool Path::createFile() const{
#ifdef OS_UNIX_LIKE
    int pfd;
    if ((pfd = open(data_.c_str(), O_WRONLY | O_CREAT | O_EXCL,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1){
        return false;
    }else{
        close(pfd);
        return true;
    }
#endif
#ifdef OS_WINDOWS
    //TODO
#endif
}

bool Path::createFolder() const{
#ifdef OS_UNIX_LIKE
    if(mkdir(data_.c_str(), S_IRWXU) == 0){
        return true;
    }else if(errno == EEXIST){
        return false;
    }else{
        std::cout << errno;
        throw PathException("unable to create folder");
    }
#endif
#ifdef OS_WINDOWS
    //TODO
#endif
}

std::list<Path> Path::children() const{
    std::list<Path> children;
#ifdef OS_UNIX_LIKE
    DIR *handle = opendir(data_.data());
    struct dirent *child = readdir(handle);
    while(child != NULL){
        if(child->d_name[0] != '.'){
            children.push_back(Path{*this, std::string{child->d_name}});
        }
        child = readdir(handle);
    }
    closedir(handle);
#endif
#ifdef OS_WINDOWS
    //TODO
#endif
    return children;
};

std::list<Path> Path::childFolders() const{
    std::list<Path> children;
#ifdef OS_UNIX_LIKE
    DIR *handle = opendir(data_.data());
    struct dirent *child = readdir(handle);
    while(child != NULL){
        if(child->d_type == DT_DIR && child->d_name[0] != '.'){
            children.push_back(Path{*this, std::string{child->d_name}});
        }
        child = readdir(handle);
    }
    closedir(handle);
#endif
#ifdef OS_WINDOWS
    //TODO
#endif
    return children;
};

std::string Path::data() const{
    return data_;
}

std::ostream &operator<<(std::ostream &output, const Core::Path &path){
    return output << path.data();
};

PathException::PathException(std::string message) : std::runtime_error(message){};
