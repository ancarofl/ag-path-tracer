//
// Created by George Lorenzetti on 1/12/18.
//

#include "precomp.h"

Triangle::Triangle(vec3 v1, vec3 v2, vec3 v3, vec3 color, Material mat) {
	this->vertices.v1 = v1;
	this->vertices.v2 = v2;
	this->vertices.v3 = v3;
	this->color = color;
	this->material = mat;
	if (mat.getMaterialType() != light) {
		this->intensity = 0.f;
	}
	else {
		this->intensity = 1.f;
	}

	vec3 vector1 = (vertices.v2 - vertices.v1);
	vec3 vector2 = (vertices.v3 - vertices.v1);
	this->normal = vector1.cross(vector2);
	this->normal.normalize();

	bounds.bounds[1].x = max(v1.x, max(v2.x, v3.x));
	bounds.bounds[0].x = min(v1.x, min(v2.x, v3.x));
	bounds.bounds[1].y = max(v1.y, max(v2.y, v3.y));
	bounds.bounds[0].y = min(v1.y, min(v2.y, v3.y));
	bounds.bounds[1].z = max(v1.z, max(v2.z, v3.z));
	bounds.bounds[0].z = min(v1.z, min(v2.z, v3.z));
	bounds.calcCentre();

}

Intersection Triangle::intersects(Ray* ray) {
	Intersection I;
	I.t = -1;
	float denom = this->normal.dot(ray->getDirection());
	if (abs(denom) > 0.01f) // your favorite epsilon
	{
		float t = (this->vertices.v1 - ray->getOrigin()).dot(normal) / denom;

		if (t < 0) {
			return I;
		}
		vec3 intersectionPoint = ray->getOrigin() + t * ray->getDirection();
		//inside outside triangle intersection test
		vec3 edge1 = this->vertices.v2 - this->vertices.v1;
		vec3 c1 = intersectionPoint - this->vertices.v1;
		vec3 c = edge1.cross(c1);
		if (this->normal.dot(c) < 0) {
			return I;
		}

		vec3 edge2 = this->vertices.v3 - this->vertices.v2;
		vec3 c2 = intersectionPoint - this->vertices.v2;
		c = edge2.cross(c2);
		if (this->normal.dot(c) < 0) {
			return I;
		}

		vec3 edge3 = this->vertices.v1 - this->vertices.v3;
		vec3 c3 = intersectionPoint - this->vertices.v3;
		c = edge3.cross(c3);
		if (this->normal.dot(c) < 0) {
			return I;
		}

		I.t = t;
		I.point = ray->getOrigin() + (I.t * ray->getDirection());
		I.normal = this->normal;
		I.normal.normalize();
		if(denom > 0){
			I.normal *= -1.0f;
		}
		I.color = this->color;
		I.specularity = this->material.getSpecularity();
		return I; // you might want to allow an epsilon here too
	}
	return I;
}

bool Triangle::shadowRayIntersects(Ray ray) {
	float denom = this->normal.dot(ray.getDirection());
	if (abs(denom) > 0.01f) // your favorite epsilon
	{
		float t = (this->vertices.v1 - ray.getOrigin()).dot(normal) / denom;

		vec3 intersectionPoint = ray.getOrigin() + t * ray.getDirection();
		//inside outside triangle intersection test
		vec3 edge1 = this->vertices.v2 - this->vertices.v1;
		vec3 c1 = intersectionPoint - this->vertices.v1;
		vec3 c = edge1.cross(c1);
		if (this->normal.dot(c) < 0) {
			return false;
		}

		vec3 edge2 = this->vertices.v3 - this->vertices.v2;
		vec3 c2 = intersectionPoint - this->vertices.v2;
		c = edge2.cross(c2);
		if (this->normal.dot(c) < 0) {
			return false;
		}

		vec3 edge3 = this->vertices.v1 - this->vertices.v3;
		vec3 c3 = intersectionPoint - this->vertices.v3;
		c = edge3.cross(c3);
		if (this->normal.dot(c) < 0) {
			return false;
		}
		if (t > 0.01f) {
			return true; // you might want to allow an epsilon here too
		}
	}
	return false;
}

vec3 Triangle::getColor() { return color; }
void Triangle::setColor(vec3 color) { this->color = color; }

vec3 Triangle::getNormal() { return this->normal; }

//todo:yea...
vec3 Triangle::getNormal(vec3 point) { return this->normal; }

TriangleVertices Triangle::getVertices() { return this->vertices; }
void Triangle::setVertices(vec3 v1, vec3 v2, vec3 v3) {
	this->vertices.v1 = v1;
	this->vertices.v2 = v2;
	this->vertices.v3 = v3;
}

//for area lights
void Triangle::getRandomPoint(vec3 &rp, unsigned int seed1, unsigned int seed2) {

}


//not used yet
float Triangle::getArea() { 
	return std::abs
	(vertices.v1.x * (vertices.v2.y - vertices.v2.z) + vertices.v1.y * 
	(vertices.v2.z - vertices.v2.x) + vertices.v1.z * 
	(vertices.v2.x - vertices.v2.y)) / 2;

	//return std::abs(x0 * (y1 - y2) + x1 * (y2 - y0) + x2 * (y0 - y1)) / 2;
}