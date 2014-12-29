/* 
 * File:   Window.h
 * Author: hans
 *
 * Created on 29 December 2014, 16:46
 */

#ifndef WINDOW_H
#define	WINDOW_H

#include "Metrics.h"

namespace Core{
    
    struct WindowSettings{
        Bounds<int> bounds;
        bool fullscreen;
    };
    
    class Window{
        
    public:
        Window();
        ~Window();
        
    };
    
}

#endif	/* WINDOW_H */

