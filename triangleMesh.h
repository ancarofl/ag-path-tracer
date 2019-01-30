//
// Created by George Lorenzetti on 1/12/18.
//

#ifndef TMPL8_2018_01_TRIANGLEMESH_H
#define TMPL8_2018_01_TRIANGLEMESH_H

#include "precomp.h"
class TriangleMesh: public Object {
public:
    TriangleMesh(int count, vector<vec3> vertices, vector<int> indices, vec3 color, Material material1);
    virtual Material getMaterial(){return material;}
    Intersection intersects(Ray* ray);
    bool shadowRayIntersects(Ray ray);
    Object** getTriangles();

	//for area lights
	void getRandomPoint(vec3 &rp, unsigned int seed1, unsigned int seed2);
	vec3 getNormal(vec3 point);
	vec3 getColor();
	float getArea();

private:
    int triangleCount;
    vector<vec3> vertices;
    vector<int> indices;
    vec3 color;
    Material material;
};


#endif //TMPL8_2018_01_TRIANGLEMESH_H
