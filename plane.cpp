//
// Created by George Lorenzetti on 25/11/18.
//

#include "precomp.h"

Plane::Plane(vec3 point, vec3 normal, vec3 color, Material material) {
	this->point = point;
	this->normal = normal;
	this->color = color;
	this->material = material;
	if (material.getMaterialType() != light) {
		this->intensity = 0.f;
	}
	else {
		this->intensity = 1.f;
	}
};

/* plane-ray intersection */
Intersection Plane::intersects(Ray *ray) {
	Intersection I;
	I.t = -1;

	float denom = normal.dot(ray->getDirection());
	if (abs(denom) > 0.01f) // your favorite epsilon
	{
		float t = (this->point - ray->getOrigin()).dot(normal) / denom;

		if (t < 0) {
			return I;
		}

		I.t = t;
		I.point = ray->getOrigin() + (I.t * ray->getDirection());
		I.normal = this->normal;
		I.normal.normalize();
		I.color = this->color;
		I.specularity = this->material.getSpecularity();

		//if not textured return the color
		if (this->material.getMaterialType() != textured) {
			return I;
		}

		//if textured calc texture color and return I
		vec3 v1 = vec3(I.normal.y, -1 * I.normal.x, 0);
		v1.normalize();
		vec3 v2 = v1.cross(I.normal);

		int texX = (v1.dot(I.point)) * 30;
		int texY = (v2.dot(I.point)) * 30;

		texX = texX % (FreeImage_GetWidth(textures[this->getMaterial().getTexture()]));
		texY = texY % (FreeImage_GetHeight(textures[this->getMaterial().getTexture()]));

		RGBQUAD pixel;
		FreeImage_GetPixelColor(textures[this->getMaterial().getTexture()], texX, texY, &pixel);
		float r = pixel.rgbRed / 255.0f;
		float g = pixel.rgbGreen / 255.0f;
		float b = pixel.rgbBlue / 255.0f;
		//cout << r << " " << g << " " << b << endl;
		I.color = vec3(r, g, b);
		return I; // you might want to allow an epsilon here too
	}
	return I;
}

bool Plane::shadowRayIntersects(Ray ray) {
	float denom = normal.dot(ray.getDirection());
	if (abs(denom) > 0.01f) // your favorite epsilon
	{
		float t = (point - ray.getOrigin()).dot(normal) / denom;
		if (t > 0.01f) return true; // you might want to allow an epsilon here too
	}
	return false;

}

//for area lights
void Plane::getRandomPoint(vec3 &rp, unsigned int seed1, unsigned int seed2) {

}

vec3 Plane::getNormal(vec3 point) {
	return vec3(0.0f, 0.0f, 0.0f);
}

vec3 Plane::getPoint() { return this->point; };
void Plane::setPoint(vec3 point) { this->point = point; };

vec3 Plane::getNormal() { return this->normal; };
void Plane::setNormal(vec3 normal) { this->normal = normal; };


void Plane::setColor(vec3 color) { this->color = color; };
vec3 Plane::getColor() { return this->color; }

float Plane::getArea() { return 0.f; }