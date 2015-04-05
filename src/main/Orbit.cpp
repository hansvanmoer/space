#include <vector>

#include "Orbit.h"

#include <list>

using namespace Game;

const Scalar RADIAL_MAX = atan(1.)*4;


bool Game::attach(OrbitalSystem *system, OrbitingBody *body, Orbit *orbit) {
    if(system && body && orbit &&(body->orbit_ == nullptr) && (system->orbits_.find(orbit) == system->orbits_.end()) && !(*orbit)){
        system->orbits_.insert(orbit);
        body->orbit_ = orbit;
        orbit->system_ = system;
        orbit->body_ = body;
        return true;
    }else{
        return false;
    }
}

bool Game::detach(OrbitingBody* body){
    if(body && body->orbit_ && *body->orbit_){
        body->orbit_->system_->orbits_.erase(body->orbit_);
        delete body->orbit_;
        body->orbit_ = nullptr;
        return true;
    }else{
        return false;
    }
}

Orbit::Orbit() : body_(), system_(){}

Orbit::~Orbit(){}

OrbitingBody *Orbit::body() const{
    return body_;
}

OrbitalSystem* Orbit::system() const {
    return system_;
}

void Orbit::update() {
    if(system_ && body_){
        doUpdate();
    }
}

Orbit::operator bool() const{
    return system_ && body_;
}

bool Orbit::operator!() const {
    return !(system_ && body_);
}

Body::Body() : position(){}


Body::~Body() {}


OrbitingBody::OrbitingBody() : orbit_(){
}

OrbitingBody::~OrbitingBody() {
    detach(this);
}

Orbit* OrbitingBody::orbit() const {
    return orbit_;
}

void OrbitingBody::orbitUpdated() {
}

OrbitalSystem::OrbitalSystem() : orbits_(){}

OrbitalSystem::~OrbitalSystem(){
    std::vector<OrbitingBody *> children{orbits_.size()};
    for(auto orbit : orbits_){
        children.push_back(orbit->body());
    }
    for(auto child : children){
        delete child;
    }
}

void OrbitalSystem::orbitUpdated() {
    OrbitingBody::orbitUpdated();
    for(auto orbit : orbits_){
        orbit->update();
    }
}

StaticOrbit::StaticOrbit(Position relativePosition) : Orbit(), relativePosition_(relativePosition){}

void StaticOrbit::doUpdate(){
    body_->position = system_->position + relativePosition_;
};

CircularOrbit::CircularOrbit(Scalar radius, Scalar radialSpeed, Scalar radialAngle)
: Orbit(), radius_(radius), radialSpeed_(radialSpeed), radialAngle_(radialAngle){}

void CircularOrbit::doUpdate(){
    radialAngle_+=radialSpeed_;
    if(radialAngle_ > angularMax()){
        radialAngle_-=angularMax();
    }
    body_->position = Position{system_->position.x+cos(radialAngle_)*radius_, system_->position.y+sin(radialAngle_)*radius_};
};