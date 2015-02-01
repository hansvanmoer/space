#include "Texture.h"

using namespace Game;

Texture::Texture() : id(0), image_(){
}

Texture::Texture(Core::Path file) : Texture(){
    load(file);
}

Texture::~Texture() {
    unload();
}

Texture::operator bool() const{
    return id;
}

bool Texture::operator !() const {
    return !id;
}

bool Texture::load(Core::Path file) {
    unload();
    std::string name{file.data()};
    if(image_.loadFromFile(name)){
        glGenTextures(1, &id);
        if(id){
            glBindTexture(GL_TEXTURE_2D, id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_.getSize().x, image_.getSize().y,0, GL_RGBA,GL_UNSIGNED_BYTE, image_.getPixelsPtr());
            return true;
        }
    }
    return false;
}

bool Texture::unload() {
    if(id){
        glDeleteTextures(1, &id);
        id=0;
        return true;
    }else{
        return false;
    }
}


