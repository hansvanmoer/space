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

#include "Star.h"

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

        struct ScrollRegion{
            bool scrolling;
            Core::Bounds<int> bounds;
            Core::Point<Scalar> vector;
            
            ScrollRegion();
            
            void check(int x,int y);
        };
        
        void handleEvents();
        
        void zoom(int delta);

        void draw();
        
        void resize(int width, int height);
        
        void loadTestScenario();
        
        void unloadTestScenario();
        
        ViewPoint viewPoint_;
        Window window_;
        ApplicationSettings settings_;
        std::atomic<bool> running_;
        ScrollRegion scrollRegions[8];
        
        StarResourceLoader starResources_;
        
        Star *star_;
    };
    
}

#endif	/* SESSION_H */

