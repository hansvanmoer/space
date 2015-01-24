#include "System.h"
#include "Data.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>

using namespace Game;
using Core::Path;

const std::string DataSystem::NAME{"data"};

Path getHomeFolder(){
#ifdef OS_UNIX_LIKE
    struct passwd *pw = getpwuid(getuid());
    return Path{pw->pw_dir};
#endif
#ifdef OS_WINDOWS
    //TODO
#endif
}

DataSystem::DataSystem(Path applicationPath) : 
        applicationPath_(applicationPath), 
        dataPath_(applicationPath, "data"),
        runtimeDataPath_(getHomeFolder(), ".spacegame"){
    dataPath_.createFolder();
    runtimeDataPath_.createFolder();
}

Path DataSystem::applicationPath(){
    return applicationPath_;
};

Path DataSystem::dataPath(){
    return dataPath_;
}

Path DataSystem::runtimeDataPath(){
    return runtimeDataPath_;
}