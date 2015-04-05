/* 
 * File:   Orbit.h
 * Author: hans
 *
 * Created on 29 January 2015, 18:35
 */

#ifndef FEATURE_H
#define	FEATURE_H

#include "Metrics.h"
#include "Graphics.h"

#include <set>

namespace Game{

    class Feature{
    public:
        virtual ~Feature();
        
        virtual const Position &position() const = 0;

        virtual void position(Position position) = 0;
                
    protected:
        Feature();
    private:
        Feature(const Feature &) = delete;
        Feature &operator=(const Feature &) = delete;
    };
    
}

#endif	/* FEATURE_H */

