#include "Window.h"

#include "Application.h"
#include "Settings.h"

#include <iostream>

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

void Window::open(std::u32string title, Core::Bounds<int> bounds, unsigned int bitsPerPixel){
    if(handle_){
        throw WindowException{"window already opened"};
    }
    if(bitsPerPixel != 24 && bitsPerPixel != 32){
        throw WindowException(std::string{"unsupported window bit depth: "} + std::to_string(bitsPerPixel));
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
    
    sf::VideoMode videoMode{size.x, size.y,bitsPerPixel};
    handle_ = new sf::Window(videoMode, std::basic_string<uint32_t>{title.begin(), title.end()}, sf::Style::Default);
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
    settings.majorVersion=2;
    settings.minorVersion=0;
    settings.antialiasingLevel=0;//static_cast<unsigned int>(videoSettings.antialiasingLevel);
}

Window::~Window(){
    delete handle_;
}
