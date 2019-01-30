//
// Created by George Lorenzetti on 1/12/18.
//

#ifndef TMPL8_2018_01_TRIANGLE_H
#define TMPL8_2018_01_TRIANGLE_H

#include "precomp.h"
struct TriangleVertices{
    vec3 v1 = vec3(0,0,0);
    vec3 v2 = vec3(0,0,0);
    vec3 v3 = vec3(0,0,0);
};

class Triangle: public Object {
public:
    Triangle(vec3 v1, vec3 v2, vec3 v3, vec3 color, Material mat);

    Intersection intersects(Ray* ray);
    bool shadowRayIntersects(Ray ray);

    vec3 getColor();
    void setColor(vec3 color);

    vec3 getNormal();
	vec3 getNormal(vec3 point);

    TriangleVertices getVertices();
    void setVertices(vec3 v1, vec3 v2, vec3 v3);

    Material getMaterial() {
        return material;
    };

	void getRandomPoint(vec3 &rp, unsigned int seed1, unsigned int seed2);
	float getArea();

	vec3 color;
	Material material;
	TriangleVertices vertices;
	vec3 normal;
	bool isTriangle = true;

private:
   
};


#endif //TMPL8_2018_01_TRIANGLE_H
