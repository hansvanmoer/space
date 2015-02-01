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
#include "Settings.h"

#include <atomic>

namespace Game{
    
    class Session{
    public:
        
        static const Scalar MIN_ZOOM_LEVEL;
    
        static const Scalar MAX_ZOOM_LEVEL;
        
        Session();

        void startEventLoop();
        
        void stop();
        
    private:

        void handleEvents();
        
        void zoom(int delta);

        void draw();
        
        ViewPoint viewPoint_;
        Window window_;
        ApplicationSettings settings_;
        std::atomic<bool> running_;
        int cumulativeZoomDelta_;
    };
    
}

#endif	/* SESSION_H */

