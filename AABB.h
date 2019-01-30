//
// Created by George Lorenzetti on 18/12/18.
//

#ifndef TMPL8_2018_01_AABB_H
#define TMPL8_2018_01_AABB_H

#include "precomp.h"

class AABB {
public:
    vec3 bounds[2];
    vec3 centrePoint;
    void calcCentre();
    bool intersect(Ray r);
    vec3 color = vec3 ((rand()%255)/255.0f, (rand()%255)/255.0f, (rand()%255)/255.0f);
};


#endif //TMPL8_2018_01_AABB_H
