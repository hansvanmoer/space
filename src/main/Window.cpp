#include "Window.h"

#include "Application.h"
#include "Settings.h"

using namespace Game;

WindowException::WindowException(std::string message) : std::runtime_error(message){}


Window::Window() : handle_(){}

void Window::activateContext(){
    if(handle_){
        handle_->setActive(true);
    }
}

void Window::deactivateContext(){
    if(handle_){
        handle_->setActive(false);
    }
}

bool Window::opened() const{
    return handle_;
}

bool Window::closed() const{
    return !handle_;
}

void Window::open(Core::UnicodeString title, Core::Bounds<int> bounds){
    if(handle_){
        throw WindowException{"window already opened"};
    }
    sf::Vector2u size{
        static_cast<unsigned int>(
            bounds.left < bounds.right ?  
                bounds.right - bounds.left : 
                bounds.left - bounds.right
        ),
        static_cast<unsigned int>(
            bounds.top < bounds.bottom ?  
                bounds.bottom - bounds.top : 
                bounds.top - bounds.bottom
        )
    };
    sf::VideoMode videoMode{size.x, size.y,32};
    handle_ = new sf::Window(videoMode, std::basic_string<uint32_t>{title.begin(), title.end()}, sf::Style::Default, createContextSettings());
    sf::Vector2i pos{bounds.left, bounds.top};
    handle_->setPosition(pos);
    handle_->setSize(size);
    handle_->setVerticalSyncEnabled(true);
}

bool Window::nextEvent(WindowEvent &event){
    return handle_->pollEvent(event);
}

void Window::render(){
    handle_->display();
}

sf::ContextSettings Window::createContextSettings(){
    VideoSettings videoSettings = ApplicationSystem<SettingsSystem>::instance().applicationSettings().videoSettings;
    sf::ContextSettings settings;
    settings.depthBits=24;
    settings.stencilBits=8;
    settings.majorVersion=3;
    settings.minorVersion=0;
    settings.antialiasingLevel=static_cast<unsigned int>(videoSettings.antialiasingLevel);
}

Window::~Window(){
    delete handle_;
}
