/* 
 * File:   Orbit.h
 * Author: hans
 *
 * Created on 29 January 2015, 18:35
 */

#ifndef FEATURE_H
#define	FEATURE_H

#include "Metrics.h"

#include <set>

namespace Game{
    
    using Scalar = double;
    
    using Position = Core::Point<Scalar>;
    
    class Feature{
    public:
        virtual ~Feature();
        
        const Position &position() const;

        void position(Position position);
    protected:
        Feature();
    private:
        Position position_;
        
        Feature(const Feature &) = delete;
        Feature &operator=(const Feature &) = delete;
    };
    
    class Orbit{
    public:
        virtual ~Orbit();
        
        virtual void update(const Position &anchor) = 0;
        
        Feature &feature();
        
        const Feature &feature() const;
        
    protected:
        Orbit(Feature &feature);
                
        Feature &feature_;
    private:
        Orbit(const Orbit &) = delete;
        Orbit &operator=(const Orbit &) = delete;
    };
    
    class StaticOrbit : public Orbit{
    public:
        StaticOrbit(Feature &feature, const Position relativePosition);
        
        void update(const Position &anchor) = 0;
    private:
        Position relativePosition_;
    };
    
    class CircularOrbit : public Orbit{
    public:
        CircularOrbit(Feature &feature, Scalar radius, Scalar radialSpeed, Scalar radialAngle = 0.);
        
        void update(const Position &anchor);
    
    private:
        Scalar radius_;
        Scalar radialSpeed_;
        Scalar radialAngle_;
    };
    
    class OrbitalSystem;
    
    struct Anchor{
        OrbitalSystem *system;
        Orbit *orbit;
        
        Anchor();
        
        Anchor(OrbitalSystem *system_, Orbit *orbit_);
        
        bool detach();
    };
    
    class OrbitalSystem : public Feature{
    public:
        virtual ~OrbitalSystem();
        
        void update(const Position &position);
        
        Anchor attach(Orbit *orbit);
        
        bool detach(Orbit *orbit);
    protected:
        OrbitalSystem();
    private:
        std::set<Orbit *> orbits_;
    };
    
}

#endif	/* FEATURE_H */

