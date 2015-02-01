#include "Feature.h"
#include "Metrics.h"

#include <math.h>

using namespace Game;

const Scalar RADIAL_MAX = atan(1.)*4;

Feature::Feature() : position_(){}

Feature::Feature(Position position) : position_(position){};

Feature::~Feature(){
}

const Position &Feature::position() const {
    return position_;
}

void Feature::position(Position position) {
    position_ = position;
}

Orbit::Orbit(Feature& feature) : feature_(feature){}

Orbit::~Orbit(){
}

const Feature& Orbit::feature() const {
    return feature_;
}

Feature &Orbit::feature(){
    return feature_;
}

StaticOrbit::StaticOrbit(Feature& feature, const Position relativePosition) : Orbit(feature), relativePosition_(relativePosition){}

void StaticOrbit::update(const Position& anchor){
    feature_.position(Position{anchor.x+relativePosition_.x,anchor.y+relativePosition_.y});
};

CircularOrbit::CircularOrbit(Feature& feature, Scalar radius, Scalar radialSpeed, Scalar radialAngle)
: Orbit(feature), radius_(radius), radialSpeed_(radialSpeed), radialAngle_(radialAngle){}

void CircularOrbit::update(const Position& anchor){
    radialAngle_+=radialSpeed_;
    if(radialAngle_ > angularMax()){
        radialAngle_-=angularMax();
    }
    feature_.position(Position{anchor.x+cos(radialAngle_)*radius_, anchor.y+sin(radialAngle_)*radius_});
};

OrbitalSystem::OrbitalSystem() : orbits_(){}

OrbitalSystem::OrbitalSystem(Position position) : Feature(position), orbits_(){}

void OrbitalSystem::update(){
    for(auto orbit : orbits_){
        orbit->update(position());
    }
};

Anchor OrbitalSystem::attach(Orbit* orbit){
    orbits_.insert(orbit).second;
    return Anchor{this, orbit};
};

bool OrbitalSystem::detach(Orbit* orbit){
    if(orbits_.erase(orbit)){
        delete orbit;
        return true;
    }else{
        return false;
    }
};

OrbitalSystem::~OrbitalSystem() {
    for(auto orbit : orbits_){
        delete orbit;
    }
}

Anchor::Anchor() : system(), orbit(){}

Anchor::Anchor(OrbitalSystem* system_, Orbit* orbit_) : system(system_), orbit(orbit_){}

bool Anchor::detach(){
    if(orbit){
        bool detached = system->detach(orbit);
        orbit = nullptr;
        return detached;
    }else{
        return false;
    }
}