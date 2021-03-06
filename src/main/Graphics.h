/* 
 * File:   Graphics.h
 * Author: hans
 *
 * Created on 31 January 2015, 14:37
 */

#ifndef GRAPHICS_H
#define	GRAPHICS_H

#include "Metrics.h"

#include <math.h>

namespace Game {

    using Scalar = double;

    using Position = Core::Point<Scalar>;

    using Dimension = Core::Size<Scalar>;

    constexpr Scalar pi() {
        return atan(1.)*4;
    };

    constexpr Scalar angularMax() {
        return atan(1.)*8;
    };

    enum class ViewMode {
        TACTICAL, STRATEGIC
    };

    struct ViewPoint {
        Position position;
        Scalar zoom;
        ViewMode mode;

        ViewPoint(Position position_, Scalar zoom_, ViewMode mode_);

        void loadProjectionMatrix() const;
    };

}

#endif	/* GRAPHICS_H */

