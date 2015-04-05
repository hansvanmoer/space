/* 
 * File:   Star.h
 * Author: hans
 *
 * Created on 30 January 2015, 21:18
 */

#ifndef STAR_H
#define	STAR_H

#include <string>

#include "Feature.h"
#include "Metrics.h"
#include "Path.h"
#include "Resource.h"
#include "Texture.h"
#include "Orbit.h"

namespace Game{
    
    class OrbitalBodyResource{
    public:
        
        std::string id;
        
        Texture strategicTexture;
        
        Texture tacticalTexture;    
        
        OrbitalBodyResource(std::string id);
        
    private:
        OrbitalBodyResource(const OrbitalBodyResource &) = delete;
        OrbitalBodyResource &operator=(const OrbitalBodyResource &) = delete;
    };
        
    class OrbitalBodyResourceLoader : public Core::ResourceBundle<std::string, OrbitalBodyResource>{
    public:
        
        void load(Core::Path path);
                
    };
    
    class StarSystem;
    
    using StarResource = OrbitalBodyResource;

    using StarResourceLoader = OrbitalBodyResourceLoader;    
    
    class Star : public OrbitalSystem{
    public:
        StarSystem *system;
        std::u32string name;
        Scalar radius;        
        const StarResource *resource;
        
        Star(StarSystem *system, std::u32string name, Scalar radius, const StarResource *resource);
        
        void draw();
        
        void update();
        
    private:
        Star(const Star &) = delete;
        Star &operator=(const Star &) = delete;
    };
    
    using PlanetResource = OrbitalBodyResource;

    using PlanetResourceLoader = OrbitalBodyResourceLoader;      
    
    class Planet : public OrbitalSystem{
    public:
        StarSystem *system;
        std::u32string name;
        Scalar radius;        
        const PlanetResource *resource;
        std::list<Planet *> moons;
        
        Planet(StarSystem *system, std::u32string name, Scalar radius, const PlanetResource *resource);
        
        ~Planet();
        
        void draw();
        
    private:
        Planet(const Planet &) = delete;
        Planet &operator=(const Planet &) = delete;
    };
    
    class StarSystem : public OrbitalSystem{
    public:    
        std::list<Star *> stars;
        std::list<Planet *> planets;
        
        StarSystem();
        
        void draw();
        
        void updateOrbits();
        
        ~StarSystem();
    private:
        StarSystem(const StarSystem &) = delete;
        StarSystem &operator=(const StarSystem &) = delete;
    };
    
}

#endif	/* STAR_H */

