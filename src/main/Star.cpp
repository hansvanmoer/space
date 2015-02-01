#include <GL/gl.h>

#include "Star.h"
#include "String.h"
#include "IO.h"

using namespace Game;

using Core::Path;

OrbitalBodyResource::OrbitalBodyResource(std::string id_) : id(id_), strategicTexture(), tacticalTexture(){};

void OrbitalBodyResourceLoader::load(Core::Path path){
    Path descriptor{path.child("descriptor")};
    if(descriptor.fileExists()){
        std::ifstream input;
        descriptor.openFile(input);
        IO::Document document{IO::open(input)};
        IO::Object object = document.rootNode().object();
        try{
            std::string id = object.getString("id");
            std::string strategic = object.getString("strategic");
            std::string tactical = object.getString("tactical");
            OrbitalBodyResource *resource = new OrbitalBodyResource(id);
            std::string file{path.child(strategic).data()};
            if(resource->strategicTexture.load(file)){
                file = path.child(tactical).data();
                if(resource->tacticalTexture.load(file)){
                    Core::ResourceBundle<std::string, OrbitalBodyResource>::add(id, resource);
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

Star::Star(StarSystem *system_, std::u32string name_, Position position_, Scalar radius_, const StarResource* resource_) 
: OrbitalSystem(position_), system(system_), name(name_), radius(radius_), resource(resource_){}

void Star::draw(){
    Position pos = position();
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, resource->tacticalTexture.id);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0);
    glVertex3d(pos.x-radius, pos.y-radius, 0);
    glTexCoord2d(1.0,0);
    glVertex3d(pos.x+radius, pos.y-radius, 0);
    glTexCoord2d(1.0,1.0);
    glVertex3d(pos.x+radius, pos.y+radius, 0);
    glTexCoord2d(0,1.0);
    glVertex3d(pos.x-radius, pos.y+radius, 0);
    glEnd();
}

Planet::Planet(StarSystem *system_, std::u32string name_, Scalar radius_, const PlanetResource* resource_) 
: OrbitalSystem(), system(system_), name(name_), radius(radius_), resource(resource_), moons(){}

Planet::~Planet(){
    for(auto moon : moons){
        delete moon;
    }
}

void Planet::add(Planet* moon, Scalar radius, Scalar radialSpeed, Scalar radialAngle) {
    moons.push_back(moon);
    attach(new CircularOrbit{*moon, radius, radialSpeed, radialAngle});
}


void Planet::draw(){
    Position pos = position();
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, resource->tacticalTexture.id);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0);
    glVertex3d(pos.x - radius, pos.y - radius, 0);
    glTexCoord2d(1.0,0);
    glVertex3d(pos.x+radius, pos.y - radius, 0);
    glTexCoord2d(1.0,1.0);
    glVertex3d(pos.x+radius, pos.y+radius, 0);
    glTexCoord2d(0,1.0);
    glVertex3d(pos.x - radius, pos.y+radius, 0);
    glEnd();
    for(auto moon : moons){
        moon->draw();
    }
}

StarSystem::StarSystem() : star(), planets(){}

StarSystem::~StarSystem(){
    delete star;
    for(auto planet : planets){
        delete planet;
    }
}

void StarSystem::add(Planet* planet, Scalar radius, Scalar radialSpeed, Scalar radialAngle) {
    planets.push_back(planet);
    star->attach(new CircularOrbit{*planet, radius, radialSpeed, radialAngle});
}

void StarSystem::update() {
    star->update();
    for(auto planet : planets){
        planet->update();
    }
}

void StarSystem::draw() {
    star->draw();
    for(auto planet : planets){
        planet->draw();
    }
}
