#include "Path.h"
#include "Settings.h"
#include "System.h"

#include <sstream>

#ifdef OS_UNIX_LIKE
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#endif

using namespace Game;

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
    return buffer.str();
}

Path::Path() : data_() {
};

Path::Path(std::string data) : data_(data) {
};

Path::Path(Path parent, std::string name) : data_(concatenate(parent.data_, name)) {
};

Path Path::child(std::string name) {
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
        throw PathException("unable to create folder");
    }
#endif
#ifdef OS_WINDOWS
    //TODO
#endif
}

std::string Path::data() const{
    return data_;
}

bool Path::openFile(std::ifstream &input) const{
    input.open(data_.c_str());
    return input.good();
}


bool Path::openFile(std::ofstream &output) const{
    output.open(data_.c_str());
    return output.good();
}

PathException::PathException(std::string message) : std::runtime_error(message){};

Path Path::applicationPath_{};

std::string createHomeFolder(){
#ifdef OS_UNIX_LIKE    
    struct passwd *pw = getpwuid(getuid());
    return std::string{pw->pw_dir};
#endif
#ifdef OS_WINDOWS
    //TODO
#endif
}

Path Path::applicationPath(){
    return applicationPath_;
};

void Path::applicationPath(Path path){
    applicationPath_ = path;
}

Path Path::dataPath(){
    return Path{applicationPath_, "data"};
}

Path Path::runtimeDataPath(){
    static Path path{createHomeFolder(), ".spacegame"};
    return path;
}
