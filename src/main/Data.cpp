#include "Data.h"

using namespace Game;

const std::string DataSystem::NAME{"data"};

std::string createHomeFolder(){
#ifdef OS_UNIX_LIKE    
    struct passwd *pw = getpwuid(getuid());
    return std::string{pw->pw_dir};
#endif
#ifdef OS_WINDOWS
    //TODO
#endif
}

DataSystem::DataSystem(Path applicationPath) : applicationPath_(applicationPath){}

Path DataSystem::applicationPath(){
    return applicationPath_;
};

void DataSystem::applicationPath(Path path){
    applicationPath_ = path;
}

Path DataSystem::dataPath(){
    return Path{applicationPath_, "data"};
}

Path DataSystem::runtimeDataPath(){
    static Path path{createHomeFolder(), ".spacegame"};
    return path;
}