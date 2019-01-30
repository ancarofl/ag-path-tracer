//
// Created by George Lorenzetti on 25/11/18.
//

#include "precomp.h"
static float getrandom(unsigned int *seed0, unsigned int *seed1) {
	*seed0 = 36969 * ((*seed0) & 65535) + ((*seed0) >> 16);  // hash the seeds using bitwise AND and bitshifts
	*seed1 = 18000 * ((*seed1) & 65535) + ((*seed1) >> 16);

	unsigned int ires = ((*seed0) << 16) + (*seed1);

	// Convert to float
	union {
		float f;
		unsigned int ui;
	} res;

	res.ui = (ires & 0x007fffff) | 0x40000000;  // bitwise AND, bitwise OR

	return (res.f - 2.f) / 2.f;
}

std::mt19937 mersenneTwister2;
std::uniform_real_distribution<float> distribution2(0.0f, 1.0f);


static float getRandom() {
	return distribution2(mersenneTwister2);
}

Sphere::Sphere(vec3 centre, float radius, vec3 color, Material m) {
	this->centre = centre;
	this->radius = radius;
	this->color = color;
	this->material = m;

	bounds.bounds[1].x = centre.x + radius;
	bounds.bounds[0].x = centre.x - radius;
	bounds.bounds[1].y = centre.y + radius;
	bounds.bounds[0].y = centre.y - radius;
	bounds.bounds[1].z = centre.z + radius;
	bounds.bounds[0].z = centre.z - radius;
	bounds.calcCentre();

	if (m.getMaterialType() != light) {
		this->intensity = 0.f;
	}
	else {
		this->intensity = 1.f;
	}
}

bool Sphere::shadowRayIntersects(Ray ray) {
	vec3 op = this->centre - ray.getOrigin();
	float t, eps = 1e-4, b = op.dot(ray.getDirection()), det = b * b - op.dot(op) + this->radius * this->radius;
	if (det < 0)
		return false;
	else {
		det = sqrt(det);
		float p = b - det > eps ? b - det : (b + det > eps ? b + det : -1);
		if (p >= 0) {
			return true;
		}
		else {
			return false;
		}
		
	}
}

vec3 Sphere::getTexture(vec3 N, vec3 IP) {
	double u = 0.5 + atan2(N.z, N.x) / (2 * PI);
	double v = 0.5 + asin(N.y) / (2 * PI);
	RGBQUAD pixel;
	float width = u * (FreeImage_GetWidth(textures[this->getMaterial().getTexture()]));
	float height = v * (FreeImage_GetHeight(textures[this->getMaterial().getTexture()]));
	FreeImage_GetPixelColor(textures[this->getMaterial().getTexture()], width - 1, height - 1, &pixel);
	float r = pixel.rgbRed / 255.0f;
	float g = pixel.rgbGreen / 255.0f;
	float b = pixel.rgbBlue / 255.0f;
	//printf("anca %f%f%f\n", r, g, b);
	return(vec3(r, g, b));
}

/*Set the intersection between ray and sphere by solving the quadratic equation for intersection points
 * This function will set in the ray, the new point and colour of intersection if an intersection closer
 * than any previous intersections are found.
 */
Intersection Sphere::intersects(Ray* ray) {
	Intersection I;
	I.t = -1;

	// solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	vec3 op = this->centre - ray->getOrigin();
	float t, eps = 1e-4, b = op.dot(ray->getDirection()), det = b * b - op.dot(op) + this->radius * this->radius;
	if (det < 0)
		return I;
	else {
		det = sqrt(det);
		I.t = b - det > eps ? b - det : (b + det > eps ? b + det : -1);
		vec3 point = ray->getOrigin() + (I.t*ray->getDirection());
		I.normal = point - this->centre;
		I.normal.normalize();
		I.point = ray->getOrigin() + (I.t * ray->getDirection());
		I.specularity = this->material.getSpecularity();
		//if not textured return the color
		if (this->material.getMaterialType() != textured) {
			I.color = this->getColor();
		}
		else {
			I.color = this->getTexture(I.normal, I.point);
		}
		return I;
	}
	return I;
}

//for area lights
void Sphere::getRandomPoint(vec3 &rp, unsigned int seed1, unsigned int seed2) {
	float u = getRandom();
	float v = getRandom();

	float cosPhi = 2.0f * u - 1.0f;
	float sinPhi = sqrt(1.0f - cosPhi * cosPhi);
	float theta = 2.0f * PI * v;

	float x = this->radius * sinPhi * sin(theta);
	float y = this->radius * cosPhi;
	float z = this->radius * sinPhi * cos(theta);

	rp = this->centre + vec3(x, y, z);
}

vec3 Sphere::getNormal(vec3 p) {
	return (p - this->centre).normalized();
}

void Sphere::translate(vec3 d, float delta) {
	mat4 translate = mat4();
	translate.cell[12] = d.x*delta;
	translate.cell[13] = d.y*delta;
	translate.cell[11] = d.z*delta;
	//cout  << delta << " -- " << centre.x << " " << centre.y << " " << centre.z << endl;
	transformationMatrix = transformationMatrix*translate;
	vec4 pos = vec4(centre, 1) * transformationMatrix;
	centre = vec3(pos.x, pos.y, pos.z);
	//cout  << centre.x << " " << centre.y << " " << centre.z << endl << " -------- " << endl;
}

vec3 Sphere::getCentre() { return this->centre; }
void Sphere::setCentre(vec3 centre) { this->centre = centre; }

vec3 Sphere::getRadius() { return this->radius; };
void Sphere::setRadius(float radius) { this->radius = radius; }

vec3 Sphere::getColor() { return this->color; }
void Sphere::setColor(vec3 color) { this->color = color; }

//A=4pir^2
float Sphere::getArea() {
	return 4.0f * PI * this->radius * this->radius;
}

