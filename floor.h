//
// Created by George Lorenzetti on 5/12/18.
//

#ifndef TMPL8_2018_01_FLOOR_H
#define TMPL8_2018_01_FLOOR_H

#include "precomp.h"

class Floor: public Object {
public:
    Floor(float z, float length, float depth, float Ox, float Oy, Material material);
	Floor(float z, float length, float depth, float Ox, float Oy, Material material, vec3 color);
    Intersection intersects(Ray* ray);
    bool shadowRayIntersects(Ray ray);

    Material getMaterial() {
        return material;
    };
    void setMaterial(Material material) { this->material = material; };
	
	/*path tracer */
	//for area lights
	void getRandomPoint(vec3 &rp, unsigned int seed1, unsigned int seed2);
	vec3 getNormal(vec3 point);
	vec3 getColor();
	float getArea();
		/*end path tracer */
	
private:
    float y;
    float length;
    float depth;
    float offsetX;
    float offsetZ;
    vec3 color = vec3(1.0f, 1.0f, 1.0f);
    Material material;
};


#endif //TMPL8_2018_01_FLOOR_H
