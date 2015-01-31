/* 
 * File:   Star.h
 * Author: hans
 *
 * Created on 30 January 2015, 21:18
 */

#ifndef STAR_H
#define	STAR_H

#include "Feature.h"

#include "Metrics.h"

#include <string>

#include "SFML/Graphics/Image.hpp"

namespace Game{
        
    class StarGraphics{
    public:
        std::string id;
        sf::Image strategicImage;
        sf::Image tacticalImage;
        sf::Image iconImage;
    private:
        StarGraphics(const StarGraphics &) = delete;
        StarGraphics &operator=(const StarGraphics &) = delete;
    };
        
    class Star : public OrbitalSystem{
    public:
        std::u32string name;
        const StarGraphics *graphics;
        Position position;
        Scalar size;
    private:
        Star(const Star &) = delete;
        Star &operator=(const Star &) = delete;
    };
    
    class Galaxy{
    public:
        std::list<Star *> stars;
    };
    
}

#endif	/* STAR_H */

