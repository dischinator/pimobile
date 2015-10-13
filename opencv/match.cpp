#include "match.hpp"

pmv::Match::Match(pmv::Object* matchedObject, double x, double y) {
    object = matchedObject;
    centerX = x;
    centerY = y;
}

pmv::Object* pmv::Match::getObject() {
    return object;
}

double pmv::Match::getCenterX() {
    return centerX;
}

double pmv::Match::getCenterY() {
    return centerY;
}

