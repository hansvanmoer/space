#include <GL/gl.h>

#include "Star.h"
#include "String.h"
#include "IO.h"

using namespace Game;

using Core::Path;

StarResource::StarResource(std::string id_) : id(id_), strategicTexture(), tacticalTexture(){};

void StarResourceLoader::load(Core::Path path){
    Path descriptor{path.child("star")};
    if(descriptor.fileExists()){
        std::ifstream input;
        descriptor.openFile(input);
        IO::Document document{IO::open(input)};
        IO::Object object = document.rootNode().object();
        try{
            std::string id = object.getString("id");
            std::string strategic = object.getString("strategic");
            std::string tactical = object.getString("tactical");
            StarResource *resource = new StarResource(id);
            std::string file{path.child(strategic).data()};
            if(resource->strategicTexture.load(file)){
                file = path.child(tactical).data();
                if(resource->tacticalTexture.load(file)){
                    Core::ResourceBundle<std::string, StarResource>::add(id, resource);
                }else{
                    delete resource;
                    throw Core::ResourceException{Core::toString("loading star resource '", id, "' unable to load tactical image from path ", file)};
                }
            }else{
                delete resource;
                throw Core::ResourceException{Core::toString("loading star resource '", id, "' unable to load strategic image from path ", file)};
            }
        }catch(std::exception &e){
            throw Core::ResourceException{Core::toString("loading star resource from path '", descriptor, "' descriptor parsing error: ", e.what())};
        }
    }

}

Star::Star(std::u32string name_, Position position_, Scalar size_, const StarResource* resource_) 
: name(name_), position(position_), size(size_), resource(resource_){}

void Star::draw(){
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, resource->tacticalTexture.id);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0);
    glVertex3d(position.x, position.y, 0);
    glTexCoord2d(1.0,0);
    glVertex3d(position.x+size, position.y, 0);
    glTexCoord2d(1.0,1.0);
    glVertex3d(position.x+size, position.y+size, 0);
    glTexCoord2d(0,1.0);
    glVertex3d(position.x, position.y+size, 0);
    glEnd();
}
