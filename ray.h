//
// Created by George Lorenzetti on 19/11/18.
//

#ifndef TMPL8_2018_01_RAY_H
#define TMPL8_2018_01_RAY_H

#pragma once

#include "precomp.h"

class Ray {
public:
    Ray(vec3 o, vec3 d);

    bool newIntersection(vec3 point);

    vec3 getOrigin();
    void setOrigin(vec3 v);

    vec3 getDirection();
    void setDirection(vec3 v);

    vec3 getIntersection();
    void setIntersection(vec3 p);

    vec3 getColor();
    void setColor(vec3 c);


    vec3 getAbsorbtion();
    void setAbsorbtion(vec3 c);

    vec3 getInverse();
    void setInverse(vec3 v);

    int getSign(int i);

	vec3 direction;
private:
    vec3 origin;
    
    vec3 intersectionPoint = (-1, -1, -1);
    vec3 color;
    vec3 materialAbsorbtion = vec3(0,0,0);
    vec3 inverseDirection;
    int sign[3];
};


#endif //TMPL8_2018_01_RAY_H
