/* 
 * File:   Window.h
 * Author: hans
 *
 * Created on 23 January 2015, 17:12
 */

#ifndef WINDOW_H
#define	WINDOW_H

#include "Metrics.h"
#include "Unicode.h"

#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include <string>
#include <stdexcept>
#include <set>

namespace Game{
    
    class WindowException : public std::runtime_error{
    public:
        WindowException(std::string message);
    };
    
    class WindowManager;

    using WindowEvent = sf::Event;
    
    class Window{
    public:
        Window();
               
        ~Window();
        
        void open(Core::UnicodeString title, Core::Bounds<int> bounds, unsigned int bitsPerPixel);
        
        void openFullScreen(std::u32string title, Core::Size<int> size);
        
        void activateContext();
        
        void deactivateContext();
        
        bool nextEvent(WindowEvent &event);
        
        void render();
        
        void close();
        
        bool opened() const;
        
        bool closed() const;
    private:
        sf::Window *handle_;
        
        sf::ContextSettings createContextSettings();
        
        void assertHandle();
        
        void validate(const Core::Bounds<int> &bounds);
        
        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;
    };
    
}


#endif	/* WINDOW_H */

