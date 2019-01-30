//
// Created by George Lorenzetti on 25/11/18.
//

#ifndef TMPL8_2018_01_PLANE_H
#define TMPL8_2018_01_PLANE_H

#include "precomp.h"

class Plane: public Object{
public:
	Plane(vec3 point, vec3 normal, vec3 color,Material material);
	Intersection intersects(Ray* ray);
	bool shadowRayIntersects(Ray ray);

	vec3 getPoint();
	void setPoint(vec3 point);

	vec3 getNormal();
	void setNormal(vec3 normal);

	vec3 getColor();
	void setColor(vec3 color);


	Material getMaterial() {
		return material;
	};
	void setMaterial(Material material) { this->material = material; };

	void getRandomPoint(vec3 &rp, unsigned int seed1, unsigned int seed2);

	vec3 getNormal(vec3 point);
	float getArea();

private:
	vec3 point;
	vec3 normal;
	vec3 color;
	Material material;
};


#endif //TMPL8_2018_01_PLANE_H
