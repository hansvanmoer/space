#include "Application.h"
#include "Data.h"
#include "Settings.h"

#include <iostream>

using namespace Game;

bool startSubSystems(int argCount, const char **args){
    std::cout << "starting subsystems" << std::endl;
    try{
        ApplicationSystem<DataSystem>::initialize(std::string{args[0]});
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
    shutdownSubSystems();
    return 0;
};