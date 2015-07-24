/* 
 * File:   MapGenerator.h
 * Author: hans
 *
 * Created on April 5, 2015, 6:42 PM
 */

#ifndef MAPGENERATOR_H
#define	MAPGENERATOR_H

#include "Star.h"
#include "Script.h"
#include "Path.h"
#include "Session.h"

#include <string>
#include <stdexcept>

namespace Game{
    
    class MapGeneratorException : public std::runtime_error{
    public:
        MapGeneratorException(std::string message);
    };
    
    class MapGenerator{
    public:
        
        static const std::string NAME;
        
        MapGenerator();
        
        ~MapGenerator();
        
        void circularOrbit(Scalar radius, Scalar angularSpeed, Scalar startAngle);
        
        void staticOrbit(Position relativePosition);
        
        StarSystem &currentSystem();
        
        void beginMap(Session *session);
        
        void nextStarSystem();
        
        void pushOrbits();
        
        void pushStar();
        
        void pushPlanet();
        
        void popOrbits();
                
        void run(std::string scriptCode);
        
        void run(Core::Path scriptPath);
        
    private:
        
        std::list<StarSystem *> systems_;
        
        StarSystem *currentStarSystem_;
        
        OrbitalSystem *currentOrbitalSystem_;
        
        Orbit *currentOrbit_;
        
        Session *session_;
        
    public:
                
        std::u32string name;
        
        Scalar radius;
        
        Position position;
        
        std::string resourceId;
        
    private:

        const OrbitalBodyResource *starResource();
        
        const OrbitalBodyResource *planetResource();
        
        void push(OrbitalSystem *orbits);
        
        MapGenerator(const MapGenerator &) = delete;
        
        MapGenerator &operator=(const MapGenerator &) = delete;
    };
    
}

#endif	/* MAPGENERATOR_H */

