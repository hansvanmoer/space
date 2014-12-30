/* 
 * File:   Window.h
 * Author: hans
 *
 * Created on 29 December 2014, 16:46
 */

#ifndef WINDOW_H
#define	WINDOW_H

#include "Metrics.h"

#include <SFML/Window/Window.hpp>

namespace Core{
    
    struct WindowSettings{
        Bounds<int> bounds;
        bool fullscreen;
        
        WindowSettings();
        WindowSettings(const Bounds<int> &bounds_, bool fullScreen);
    };
    
    class Window{
    private:
        sf::Window handle_;
    public:
        Window();
        ~Window();
        
    };
    
}

#endif	/* WINDOW_H */

