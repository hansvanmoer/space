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

namespace Game{
        
    class StarResource{
    public:
        
        std::string id;
        
        Texture strategicTexture;
        
        Texture tacticalTexture;    
        
        StarResource(std::string id);
        
    private:
        StarResource(const StarResource &) = delete;
        StarResource &operator=(const StarResource &) = delete;
    };
    
    class StarResourceLoader : public Core::ResourceBundle<std::string, StarResource>{
    public:
        
        void load(Core::Path path);
                
    };
        
    class Star : public OrbitalSystem{
    public:
        std::u32string name;
        Position position;
        Scalar size;        
        const StarResource *resource;
        
        Star(std::u32string name, Position position, Scalar size, const StarResource *resource);
        
        void draw();
        
    private:
        Star(const Star &) = delete;
        Star &operator=(const Star &) = delete;
    };
    
}

#endif	/* STAR_H */

