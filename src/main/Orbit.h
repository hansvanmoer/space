/* 
 * File:   Orbit.h
 * Author: hans
 *
 * Created on March 10, 2015, 4:17 PM
 */

#ifndef ORBIT_H
#define	ORBIT_H

#include "Graphics.h" 

#include <set>

namespace Game{
    
    class Body{
    public:
        Position position;
        
        Body();
        
        virtual ~Body();
    private:
        Body(const Body &) = delete;
        Body &operator=(const Body &) = delete;
    };
    
    class OrbitingBody;
    
    class OrbitalSystem;
    
    class Orbit;
    
    bool attach(OrbitalSystem *system, OrbitingBody *body, Orbit *orbit);
    
    bool detach(OrbitingBody *body);
    
    class Orbit{
    public:
        ~Orbit();
        
        OrbitalSystem *system() const;
        
        OrbitingBody *body() const;
        
        bool operator!() const;
        
        operator bool() const;
        
        void update();
        
    protected:
        
        Orbit();
        
        virtual void doUpdate() = 0;
        
        OrbitalSystem *system_;
        OrbitingBody *body_;
    private:
        
        friend bool attach(OrbitalSystem *, OrbitingBody *, Orbit *);
        friend bool detach(OrbitingBody *);
        
        Orbit(const Orbit &) = delete;
        
        Orbit &operator=(const Orbit &) = delete;
    };
    
    class StaticOrbit : public Orbit{
    public:
        StaticOrbit(Position relativePosition);
        
    protected:
        void doUpdate();
        
    private:
        Position relativePosition_;
    };
    
    class CircularOrbit : public Orbit{
    public:
        CircularOrbit(Scalar radius, Scalar radialSpeed, Scalar radialAngle = 0.);
        
    protected:
        void doUpdate();
        
    private:
        Scalar radius_;
        Scalar radialSpeed_;
        Scalar radialAngle_;
    };
    
    class OrbitingBody : public Body{
    public:
        
        OrbitingBody();
        
        ~OrbitingBody();
        
        Orbit *orbit() const;
    protected:
        virtual void orbitUpdated();
        
    private:
        Orbit *orbit_;
        
        friend bool attach(OrbitalSystem *, OrbitingBody *, Orbit *);
        friend bool detach(OrbitingBody *);
    };
    
    class OrbitalSystem : public OrbitingBody{
    public:
        
        OrbitalSystem();
        
        ~OrbitalSystem();
        
        const std::set<Orbit *> &orbits() const;
    protected:
        
        virtual void orbitUpdated();
        
    private:
        std::set<Orbit*> orbits_;
        
        friend bool attach(OrbitalSystem *, OrbitingBody *, Orbit *);
        friend bool detach(OrbitingBody *);
    };
    
}

#endif	/* ORBIT_H */

