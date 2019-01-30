//
// Created by George Lorenzetti on 25/11/18.
//

#ifndef TMPL8_2018_01_SPHERE_H
#define TMPL8_2018_01_SPHERE_H

#include "precomp.h"

class Sphere: public Object {
public:
	Sphere(vec3 centre, float radius, vec3 color, Material material);
	Intersection intersects(Ray* ray);
	bool shadowRayIntersects(Ray ray);
	vec3 getTexture(vec3 N, vec3 IP);

	vec3 getCentre();
	void setCentre(vec3 centre);

	vec3 getRadius();
	void setRadius(float radius);

	vec3 getColor();
	void setColor(vec3 color);

	Material getMaterial() {
		return material;
	};

	void setMaterial(Material material) { this->material = material; };

	void getRandomPoint(vec3 &rp, unsigned int seed1, unsigned int seed2);
	vec3 getNormal(vec3 point);
	float getArea();

	float radius;
	mat4 transformationMatrix;
	void translate(vec3 d, float delta);


private:
	vec3 centre;
	vec3 color;
	Material material;
};


#endif //TMPL8_2018_01_SPHERE_H
