/* 
 * File:   Texture.h
 * Author: hans
 *
 * Created on 01 February 2015, 19:36
 */

#ifndef TEXTURE_H
#define	TEXTURE_H

#include "SFML/Graphics/Image.hpp"

#include "GL/gl.h"

#include "Path.h"

namespace Game{
    
    class Texture{
    public:
        GLuint id;
        
        Texture();
        
        Texture(Core::Path file);
        
        ~Texture();

        bool load(Core::Path file);
        
        bool unload();
        
        operator bool() const;
        
        bool operator!() const;
        
    private:
        sf::Image image_;
        
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;
    };
    
}

#endif	/* TEXTURE_H */

