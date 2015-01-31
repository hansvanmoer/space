/* 
 * File:   Session.h
 * Author: hans
 *
 * Created on 31 January 2015, 15:55
 */

#ifndef SESSION_H
#define	SESSION_H

#include "Graphics.h"
#include "Window.h"

#include <atomic>

namespace Game{
    
    class Session{
    public:
        Session();

        void startEventLoop();
        
        void stop();
        
    private:

        void handleEvents();
        
        void zoom(int ticks);

        void draw();
        
        ViewPoint viewPoint_;
        Window window_;
        std::atomic<bool> running_;
    };
    
}

#endif	/* SESSION_H */

