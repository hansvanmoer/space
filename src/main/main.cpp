#include "Application.h"
#include "Data.h"
#include "Settings.h"
#include "Window.h"
#include "Path.h"
#include "Module.h"
#include "CharacterBuffer.h"

#include <iostream>
#include <map>
#include <chrono>
#include <thread>

#include "Graphics.h"
#include "Session.h"

#include "Script.h"
#include "MapGenerator.h"
#include "IO.h"

using namespace Game;
using Core::Path;
using Core::Language;

const std::string DEFAULT_MODULE{"space"};

bool startSubSystems(int argCount, const char **args) {
    std::cout << "starting subsystems" << std::endl;
    try {
        Path executablePath{args[0]};
#ifdef RUNTIME_DATA_PATH
        executablePath = executablePath.parent().parent();
#endif
        ApplicationSystem<DataSystem>::initialize(executablePath.parent());
        ApplicationSystem<SettingsSystem>::initialize();
        ApplicationSystem<Script::ScriptSystem>::initialize();
        std::cout << "all subsystems started" << std::endl;
    } catch (ApplicationException &e) {
        std::cout << "a subsystem did not properly start: " << e.what() << std::endl;
        std::cout << "closing down application" << std::endl;
    }
}

void loadModule(std::string moduleName) {
    ApplicationSystem<ModuleLoader>::initialize();
    ModuleLoader &moduleLoader = ApplicationSystem<ModuleLoader>::instance();
    Path modulesPath{ApplicationSystem<DataSystem>::instance().dataPath().child("module")};
    try {
        moduleLoader.addModules(modulesPath);
        moduleLoader.loadModule("space");
    } catch (ModuleException &e) {
        std::cout << "unable to load module" << std::endl << e.what() << std::endl;
        throw;
    }
}

template<typename System> void shutdownSubSystem(){
    try {
        std::cout << "stopping subsystem '" << System::NAME << "'" << std::endl;
        ApplicationSystem<System>::shutdown();
    } catch (ApplicationException &e) {
        std::cout << "subsystem '" << System::NAME << "' did not stop properly: " << e.what() << std::endl;
        std::cout << "closing down application" << std::endl;
    }
}

void shutdownSubSystems() {
    std::cout << "stopping subsystems" << std::endl;
    shutdownSubSystem<DataSystem>();
    shutdownSubSystem<SettingsSystem>();
    shutdownSubSystem<Script::ScriptSystem>();
    std::cout << "all subsystems stopped" << std::endl;
}

void testScript(){
    //Game::testMapGenerator();
    IO::Properties props;
    IO::loadProperties(ApplicationSystem<DataSystem>::instance().dataPath().child("properties"), props);
    for(auto prop : props){
        std::cout <<"'"<< prop.first << "' : '" << prop.second << "'" << std::endl;
    }
};

int main(int argCount, const char **args) {
    startSubSystems(argCount, args);
    try {
        ApplicationSystem<SettingsSystem>::instance().load();
    } catch (SettingsException &e) {
        std::cout << "unable to load application settings: " << e.what() << std::endl;
        std::cout << "loading defaults" << std::endl;
    }
    loadModule("space");
    testScript();
    Session session;
    session.startEventLoop();
    try {
        ApplicationSystem<SettingsSystem>::instance().save();
    } catch (SettingsException &e) {
        std::cout << "unable to save application settings: " << e.what() << std::endl;
    }
    shutdownSubSystems();

    return 0;
};