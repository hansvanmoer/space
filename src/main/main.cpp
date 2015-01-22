#include "Application.h"
#include "Data.h"
#include "Settings.h"

#include <iostream>

using namespace Game;

bool startSubSystems(int argCount, const char **args){
    std::cout << "starting subsystems" << std::endl;
    try{
        Path executablePath{args[0]};
        ApplicationSystem<DataSystem>::initialize(executablePath.parent());
        ApplicationSystem<SettingsSystem>::initialize();
        std::cout << "all subsystems started" << std::endl;
    }catch(ApplicationException &e){
        std::cout << "a subsystem did not properly start: " << e.what() << std::endl;
        std::cout << "closing down application" << std::endl;
    }
}

void shutdownSubSystems(){
    std::cout << "stopping subsystems" << std::endl;
    try{
        ApplicationSystem<DataSystem>::shutdown();
    }catch(ApplicationException &e){
        std::cout << "a subsystem did not properly stop: " << e.what() << std::endl;
        std::cout << "closing down application" << std::endl;
    }
    try{
        ApplicationSystem<SettingsSystem>::shutdown();
    }catch(ApplicationException &e){
        std::cout << "a subsystem did not properly stop: " << e.what() << std::endl;
        std::cout << "closing down application" << std::endl;
    }
    std::cout << "all subsystems stopped" << std::endl;
}


int main(int argCount, const char **args){
    startSubSystems(argCount, args);
    try{
        ApplicationSystem<SettingsSystem>::instance().load();
    }catch(SettingsException &e){
        std::cout << "unable to load application settings: " << e.what() << std::endl;
        std::cout << "loading defaults" << std::endl;
    }
    
    try{
        ApplicationSystem<SettingsSystem>::instance().save();
    }catch(SettingsException &e){
        std::cout << "unable to save application settings: " << e.what() << std::endl;
    }
    shutdownSubSystems();
    return 0;
};