//
// Created by George Lorenzetti on 19/11/18.
//

#include "precomp.h"

Ray::Ray(vec3 o, vec3 d){
    this->origin = o;
    //normalise d first
    d.normalize();
    this->direction = d;
    color = vec3(127, 240, 255);

    inverseDirection.x = 1 / direction.x;
    inverseDirection.y = 1 / direction.y;
    inverseDirection.z = 1 / direction.z;

    sign[0] = (inverseDirection.x < 0);
    sign[1] = (inverseDirection.y < 0);
    sign[2] = (inverseDirection.z < 0);
}

//this function is used when an intersection is found. It tests to see if the new intersection is close to the screen than the current one
bool Ray::newIntersection(vec3 point) {

    //first check if this is the first intersection found or not
    if(this->intersectionPoint.x == -1 && this->intersectionPoint.y == -1 && this->intersectionPoint.z == -1){
        this->intersectionPoint = point;
        return true;
    }

    //calc and compare distances
    float distanceOld = sqrt(pow(this->intersectionPoint.x - this->origin.x, 2) + pow(this->intersectionPoint.y - this->origin.y, 2) + pow(this->intersectionPoint.z - this->origin.z, 2));
    float distanceNew = sqrt(pow(point.x - this->origin.x, 2) + pow(point.y - this->origin.y, 2) + pow(point.z - this->origin.z, 2));

    //return false if new instersction isnt closer
    if(distanceOld < distanceNew){
        return false;
    }

    //if new is closer then update intersection and return true
    this->intersectionPoint = point;
    return true;
}

vec3 Ray::getOrigin() { return origin; }
void Ray::setOrigin(vec3 v) { origin = v; }

vec3 Ray::getDirection() { return direction; }
void Ray::setDirection(vec3 v) {
    //normalise v first
    v.normalize();
    direction = v;
}

vec3 Ray::getIntersection() { return this->intersectionPoint; }
void Ray::setIntersection(vec3 p) { this->intersectionPoint = p; }

vec3 Ray::getColor() { return this->color; }
void Ray::setColor(vec3 c) { this->color = c; }

vec3 Ray::getAbsorbtion() { return this->materialAbsorbtion; }
void Ray::setAbsorbtion(vec3 c) { this->materialAbsorbtion = c; }

vec3 Ray::getInverse(){ return this->inverseDirection; }
void Ray::setInverse(vec3 v){ this->inverseDirection = v; }

int Ray::getSign(int i){ return sign[i]; }