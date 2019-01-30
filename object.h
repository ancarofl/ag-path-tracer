//
// Created by George Lorenzetti on 25/11/18.
//

#ifndef TMPL8_2018_01_OBJECT_H
#define TMPL8_2018_01_OBJECT_H

#include "precomp.h"

struct Intersection{
    float t;
    vec3 point;
    vec3 normal;
    vec3 color;
    float specularity;
	bool skybox = false;
};

class Object {
public:
    virtual Intersection intersects(Ray* ray) = 0;
    virtual bool shadowRayIntersects(Ray ray) = 0;
    virtual Material getMaterial() = 0;
    AABB bounds;
	float intensity;
	virtual void getRandomPoint(vec3 &rp, unsigned int seed1, unsigned int seed2) = 0;
	virtual vec3 getNormal(vec3 point) = 0; //normal to surface
	virtual vec3 getColor() = 0;
	virtual float getArea() = 0;
	bool isTriangle = false;
};


#endif //TMPL8_2018_01_OBJECT_H
