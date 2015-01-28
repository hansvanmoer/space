#include "Application.h"
#include "Data.h"
#include "Settings.h"
#include "Window.h"
#include "Path.h"
#include "Module.h"
#include "CharacterBuffer.h"

#include <iostream>
#include <map>

using namespace Game;
using Core::Path;
using Core::Language;

bool startSubSystems(int argCount, const char **args){
    std::cout << "starting subsystems" << std::endl;
    try{
        Path executablePath{args[0]};
#ifdef RUNTIME_DATA_PATH
        executablePath = executablePath.parent().parent();
#endif
        ApplicationSystem<DataSystem>::initialize(executablePath.parent());
        ApplicationSystem<SettingsSystem>::initialize();
        std::cout << "all subsystems started" << std::endl;
    }catch(ApplicationException &e){
        std::cout << "a subsystem did not properly start: " << e.what() << std::endl;
        std::cout << "closing down application" << std::endl;
    }
}

void loadModule(){
    Path modulesPath{ApplicationSystem<DataSystem>::instance().dataPath().child("module")};
    try{
        ModuleLoader moduleLoader;
        moduleLoader.addModules(modulesPath);
        Module *module{moduleLoader.loadModule("space")};
        std::cout << "loading module " << module->id() << std::endl;
        std::list<Core::Path> paths = module->paths();
        for(auto i = paths.begin(); i != paths.end(); ++i){
            std::cout << (*i) << std::endl;
        }
        std::list<const Core::Language *> languages = module->languages();
        for(auto i = languages.begin(); i != languages.end(); ++i){
            std::cout << "language " << (*i)->id() << ", parent: " << (*i)->parent() << std::endl;
        }
        delete module;
    }catch(ModuleException &e){
        std::cout << "unable to load module" << std::endl << e.what() << std::endl;
        throw;
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

void startEventLoop(){
    Window window;
    std::string title{"Main window"};
    window.open(Core::UnicodeString{title.begin(), title.end()}, Core::Bounds<int>{0,800,0,600}, 24);
    window.activateContext();
    WindowEvent event;
    bool running = true;
    while(running){
        while(window.nextEvent(event)){
            if(event.type == sf::Event::Closed){
                running=false;
            }else if(event.type == sf::Event::Resized){
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }
        window.render();
    }
}

int main(int argCount, const char **args){
    startSubSystems(argCount, args);
    try{
        ApplicationSystem<SettingsSystem>::instance().load();
    }catch(SettingsException &e){
        std::cout << "unable to load application settings: " << e.what() << std::endl;
        std::cout << "loading defaults" << std::endl;
    }
    loadModule();
    //startEventLoop();
    
    try{
        ApplicationSystem<SettingsSystem>::instance().save();
    }catch(SettingsException &e){
        std::cout << "unable to save application settings: " << e.what() << std::endl;
    }
    shutdownSubSystems();
    return 0;
};