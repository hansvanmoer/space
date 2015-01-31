#include <GL/gl.h>

#include "Graphics.h"
#include "Metrics.h"

using namespace Game;

ViewPoint::ViewPoint(Position position_, Scalar zoom_, ViewMode mode_) : position(position_), zoom(zoom_), mode(mode_){
}

void ViewPoint::loadProjectionMatrix() const{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glTranslated(position.x, position.y, 0);
    glScaled(zoom,zoom,1.0);
}


