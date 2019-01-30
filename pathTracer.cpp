//
// Created by George Lorenzetti on 13/1/19.
//

#include "precomp.h"

std::mt19937 mersenneTwister;
std::uniform_real_distribution<double> distribution(-APERTURE, APERTURE);

static double getrandomdbl() {
	return distribution(mersenneTwister);
}

// random number generator taken from here: http://raytracey.blogspot.com/2015/10/gpu-path-tracing-tutorial-1-drawing.html
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

void PathTracer::calculateRays(Surface* screen, Camera camera) {
	//get the screen's aspect ratio
	float aspectRaio = (float)screen->GetWidth() / (float)screen->GetHeight();

	//calculate homogenous coords of camera point and 3 corner points of the screen in world co-ordinate space
	float topLeftx = ((2 * (0 / screen->GetWidth())) - 1) * tan((camera.getFOV() / 2) * (PI / 180)) * aspectRaio;
	float topLefty = (1 - (2 * (0 / screen->GetHeight()))) * tan((camera.getFOV() / 2) * (PI / 180));
	vec4 topLeft = vec4(topLeftx, topLefty, -1, 1);
	this->topLeft = camera.getMatrix()*topLeft;

	float topRightx = ((2 * (screen->GetWidth() / screen->GetWidth())) - 1) * tan((camera.getFOV() / 2) * (PI / 180)) * aspectRaio;
	float topRighty = (1 - (2 * (0 / screen->GetHeight()))) * tan((camera.getFOV() / 2) * (PI / 180));
	vec4 topRight = vec4(topRightx, topRighty, -1, 1);
	this->topRight = camera.getMatrix()*topRight;

	float bottomLeftx = ((2 * (0 / screen->GetWidth())) - 1) * tan((camera.getFOV() / 2) * (PI / 180)) * aspectRaio;
	float bottomLefty = (1 - (2 * (screen->GetHeight() / screen->GetHeight()))) * tan((camera.getFOV() / 2) * (PI / 180));
	vec4 bottomLeft = vec4(bottomLeftx, bottomLefty, -1, 1);
	this->bottomLeft = camera.getMatrix()*bottomLeft;

	//calculate x,y,x step size for pixel interpolation
	vec3 horizontalStep = vec3(this->topRight.x - this->topLeft.x, this->topRight.y - this->topLeft.y, this->topRight.z - this->topLeft.z);
	this->horizontalStep = horizontalStep * ((float)1 / screen->GetWidth());

	vec3 verticalStep = vec3(this->bottomLeft.x - this->topLeft.x, this->bottomLeft.y - this->topLeft.y, this->bottomLeft.z - this->topLeft.z);
	this->verticalStep = verticalStep * ((float)1 / screen->GetHeight());

	int counter = 0;
	//for each pixel, calculated it's coordinates in Camera space and then create a ray in screen space
}

Intersection PathTracer::nearestIntersection(Ray ray, Object** objects, int objectCount, Material &mt) {
	Intersection I;
	I.t = -1;
	for (int i = 0; i < objectCount; i++) {
		Intersection newIntersection = objects[i]->intersects(&ray);
		if (newIntersection.t > 0 && (I.t == -1 || I.t > newIntersection.t)) {
			I = newIntersection;
			mt = objects[i]->getMaterial();
		}
	}
	if (I.t < 0) {
		I.color = vec3(0.0f, 0.0f, 0.0f);
	}
	return I;
}
vec3 PathTracer::trace(Object** objects, int objectCount, Object** areaLights, int areaLightCount, float totalLightArea, Ray ray, int iterations, vec3 &result, bool lastSpecular) {
	if (useRR == false && iterations > MAXDEPTH) {
		return vec3(0.0f, 0.0f, 0.0f);
	}
	iterations++;

	// depth of field
	if (useDOF) {
		vec3 focal_point = ray.getOrigin() + ray.getDirection() * FOCALLENGTH;
		double x = getrandomdbl(); double y = getrandomdbl(); double z = getrandomdbl();
		vec3 new_origin = vec3(x, y, z) + ray.getOrigin();
		vec3 new_direction = focal_point - new_origin;
		ray.setOrigin(new_origin);
		ray.setDirection(new_direction);
	}

	Material mt, lightMt;
	Intersection I, lightI;

	if (!useBVH) {
		I = nearestIntersection(ray, objects, objectCount, mt);
	}
	else {
		I = bvh.getRoot().traverse(ray, objects, bvh.getIndices(), mt);
	}

	// get light intersection => if it is closer than the object intersection then it becomes the closest intersection
	lightI = nearestIntersection(ray, areaLights, areaLightCount, lightMt);
	if (lightI.t > 0 && (I.t == -1 || lightI.t < I.t)) {
		I = lightI;
		mt = lightMt;
	}

	// if no intersection found => stop bouncing
	if (I.t < 0) {
		if (hasSKYBOX == true && (iterations == 1 || lastSpecular)) {
			float u = (ray.direction.x + 1.0f) / 2.0f;
			float v = (ray.direction.y + 1.0f) / 2.0f;
			RGBQUAD pixel;
			float width = u * (FreeImage_GetWidth(textures[skyboxIndex]));
			float height = v * (FreeImage_GetHeight(textures[skyboxIndex]));
			FreeImage_GetPixelColor(textures[skyboxIndex], width - 1, height - 1, &pixel);
			float r = pixel.rgbRed / 255.0f;
			float g = pixel.rgbGreen / 255.0f;
			float b = pixel.rgbBlue / 255.0f;
			return(vec3(r, g, b));
		}
		else if (SCENE == 4) {
			return vec3(0.5f, 0.5f, 0.8f);
		}
		return vec3(0.0f, 0.0f, 0.0f);
	}

	// NEE exceptions
	if (useNEE) {
		// if intersection is with a light => stop bouncing
		if (mt.getMaterialType() == light) {
			// if first bounce or if previous bounce specular => return light emission
			if (iterations == 1 || lastSpecular) {
				return vec3(mt.emission, mt.emission, mt.emission);
			}
			else {
				return vec3(0.0f, 0.0f, 0.0f);
			}
		}
	}

	// if intersection is with a light => add light emission
	if (!useNEE) {
		if (mt.getMaterialType() == light) {
			return vec3(mt.emission, mt.emission, mt.emission);
		}
	}

	// Russian Roulette
	float rrf = 1.0f;
	if (useRR == true) {
		if (iterations > MAXDEPTH / 2) {
			float rrStopProbability = 0.0f;
			rrStopProbability = max(max(I.color.x, max(I.color.y, I.color.z)), 0.1f);
			if (getrandom(&seed1, &seed2) <= rrStopProbability) {
				return vec3(0.0f, 0.0f, 0.0f);
			}
			rrf = 1.0f / (1.0f - rrStopProbability);
		}
	}

	// continue random walk <==> indirect illumination (light reflected towards the intersection point by other objects)
	// diffuse object => choose a bounce direction
	if (mt.getMaterialType() == diffuse || mt.getMaterialType() == textured) {

		//define the probability distribution function and inverse it (so can be multiplied in equation later not divided)
		float inversePDF = 0.0f;

		if (useIS) {
			ray.setDirection(cosineWeightedSample(I.normal.normalized()));
			inversePDF = PI / ray.getDirection().dot(I.normal);
		}
		else {
			ray.setDirection(sampleHemisphere(I.normal).normalized());
			inversePDF = DOUBLEPI;
		}

		ray.setOrigin(I.point + (0.001f * ray.getDirection()));


		float geometryFactor = ray.getDirection().dot(I.normal);
		//cout << geometryFactor << endl;

		vec3 tmp = 0;
		vec3 indirectIllumination = trace(objects, objectCount, areaLights, areaLightCount, totalLightArea, ray, iterations, tmp, false);
		vec3 directIlluminationVal = 0;

		if (useNEE) {
			vec3 BRDF = vec3(I.color.x * INVPI, I.color.y * INVPI, I.color.z * INVPI);
			directIlluminationVal = directIllumination(objects, objectCount, areaLights, areaLightCount, I, BRDF, totalLightArea);
			return (inversePDF * BRDF * indirectIllumination * geometryFactor * rrf) + directIlluminationVal;
		}
		else {
			directIlluminationVal = vec3(I.color.x * INVPI, I.color.y * INVPI, I.color.z * INVPI); // BRDF
			return  inversePDF * directIlluminationVal * indirectIllumination * geometryFactor * rrf;
		}
	}

	if (mt.getMaterialType() == reflective) {
		vec3 reflection = ray.getDirection() - (2 * ray.getDirection().dot(I.normal)*I.normal);
		reflection.normalize();
		Ray reflectedRay = Ray(I.point + (0.0001*reflection), reflection);

		vec3 tmp = 0;
		vec3 reflectedLight = trace(objects, objectCount, areaLights, areaLightCount, totalLightArea, reflectedRay, iterations, tmp, true);
		return reflectedLight;
	}

	if (mt.getMaterialType() == snell) {
		Ray refractedRay = Ray(I.point, I.point);
		refractedRay.setAbsorbtion(mt.getAbsorbtion());
		vec3 refractedDirection = refract(ray.getDirection(), I.normal, mt.getIndex(), &refractedRay);
		refractedRay.setOrigin(I.point + (0.0001f*refractedDirection));
		refractedRay.setDirection(refractedDirection);
		vec3 tmp = 0;
		vec3 refractedLight = trace(objects, objectCount, areaLights, areaLightCount, totalLightArea, refractedRay, iterations, tmp, true);
		return refractedLight;
	}

	if (mt.getMaterialType() == fresnel || mt.getMaterialType() == beer) {
		float fresnel = fresnelValue(ray.getDirection(), I.normal, mt.getMaterialType());

		float rand1 = getrandom(&seed1, &seed2);

		vec3 tmp = 0;
		if (rand1 < fresnel) {
			vec3 reflection = ray.getDirection() - (2 * ray.getDirection().dot(I.normal)*I.normal);
			reflection.normalize();
			Ray reflectedRay = Ray(I.point + (0.0001*reflection), reflection);

			return trace(objects, objectCount, areaLights, areaLightCount, totalLightArea, reflectedRay, iterations, tmp, true);
		}
		else {
			Ray refractedRay = Ray(I.point, I.point);
			refractedRay.setAbsorbtion(mt.getAbsorbtion());
			vec3 refractedDirection = refract(ray.getDirection(), I.normal, mt.getIndex(), &refractedRay);
			refractedRay.setOrigin(I.point + (0.0001f*refractedDirection));
			refractedRay.setDirection(refractedDirection);

			vec3 refractedLight = trace(objects, objectCount, areaLights, areaLightCount, totalLightArea, refractedRay, iterations, tmp, true);
			if (mt.getMaterialType() == beer) {
				vec3 diff = I.point - ray.getOrigin();
				float length = diff.length();
				refractedLight.x *= exp(-1 * ray.getAbsorbtion().x * length);
				refractedLight.y *= exp(-1 * ray.getAbsorbtion().y * length);
				refractedLight.z *= exp(-1 * ray.getAbsorbtion().z * length);
			}
			return refractedLight;
		}
	}

	// the higher the phongFactor => the more specular the material
	if (mt.getMaterialType() == phong) {
		// get new ray origin and direction around the perfect specular reflection
		float phi = DOUBLEPI * getrandom(&seed1, &seed2);
		float rnd2 = getrandom(&seed1, &seed2);
		int phong_exponent = mt.phongFactor;
		float cosTheta = powf(1 - rnd2, 1.0f / (phong_exponent + 1));
		float sinTheta = sqrtf(1 - cosTheta * cosTheta);
		vec3 w = (ray.direction - (I.normal * 2.0f * dot(I.normal, ray.direction))).normalized();
		vec3 u, v = 0;
		if (fabs(w.x) > 0.9) {
			u = vec3{ 0.0f, 1.0f, 0.0f };
		}
		else {
			u = vec3{ 1.0f, 0.0f, 0.0f };
		}
		u = normalize(cross(u, w));
		v = cross(w, u);
		vec3 newDir = (u * cosf(phi) * sinTheta + v * sinf(phi) * sinTheta + w * cosTheta).normalized();
		ray.setDirection(newDir);
		ray.setOrigin(I.point + (0.001f * w));
		// calculate light
		vec3 tmp = 0;
		vec3 indirectIllumination = trace(objects, objectCount, areaLights, areaLightCount, totalLightArea, ray, iterations, tmp, false);
		float inversePDF = DOUBLEPI / ((phong_exponent + 2) * powf(cosTheta, phong_exponent));
		// BRDF = albedo diffuse / PI + albedo specular * PDF
		vec3 BRDF = I.color * (0.5 * INVPI + 0.5 * 1 / inversePDF);
		return inversePDF * BRDF * indirectIllumination * rrf;
	}
	return result;
};


// direct illumination for NEE <==> send a ray directly to a random light source => if sth is in the way then reject it (return black)
vec3 PathTracer::directIllumination(Object** objects, int objectCount, Object** areaLights, int areaLightCount, Intersection I, vec3 BRDF, float totalLightArea) {
	vec4 areaLight = areaWeightedRandomLightAndPoint(areaLights, areaLightCount, totalLightArea);
	int areaLightIndex = areaLight.w;
	vec3 areaLightPoint = vec3(areaLight.x, areaLight.y, areaLight.z);
	vec3 L = areaLightPoint - I.point;

	vec3 Nl = areaLights[areaLightIndex]->getNormal(areaLightPoint);
	Nl.normalize();

	float distanceSquared = L.length() * L.length();
	L.normalize();

	float geometryFactor = dot(I.normal, L);
	float dotNlminusL = dot(Nl, (-1.0f * L));

	bool visible;
	if (geometryFactor > 0.0f && dotNlminusL > 0.0f) {
		Ray shadowRay = Ray(I.point + (0.001f*L), L);
		visible = testShadow(shadowRay, objects, objectCount, areaLights, areaLightCount);
		if (visible) {
			//add distance attenuation for the lights. Not sure if this is correct.
			Material lightMaterial = areaLights[areaLightIndex]->getMaterial();
			float Area = areaLights[areaLightIndex]->getArea();
			float solidAngle = dotNlminusL * (Area / distanceSquared);
			float inversePDF = (totalLightArea / Area);
			return BRDF * lightMaterial.emission * solidAngle * geometryFactor * inversePDF;
		}
	}
	return vec3(0, 0, 0);;
}

bool PathTracer::testShadow(Ray sray, Object** objects, int objectCount, Object** areaLights, int areaLightCount) {
	Material mt, lightMt;
	Intersection I, lightI;

	if (!useBVH) {
		I = nearestIntersection(sray, objects, objectCount, mt);
	}
	else {
		I = bvh.getRoot().traverse(sray, objects, bvh.getIndices(), mt);
	}

	if (I.t < 0) {
		return true;
	}

	// get light intersection => if it is closer than the object intersection then it becomes the closest intersection
	lightI = nearestIntersection(sray, areaLights, areaLightCount, lightMt);
	if (lightI.t > 0 && (I.t == -1 || lightI.t < I.t)) {
		return true;
	}

	return false;
}

vec3 PathTracer::refract(vec3 incidence, vec3 normal, float refractiveIndex, Ray* ray) {
	//calc cosTheta1
	float cosTheta1 = normal.dot(incidence);
	if (cosTheta1 > 1) {
		cosTheta1 = 1;
	}
	else if (cosTheta1 < -1) {
		cosTheta1 = -1;
	}

	//refractive indexes
	float n1 = 1;
	float n2 = refractiveIndex;


	if (cosTheta1 < 0) {
		cosTheta1 = -1 * cosTheta1;
	}
	else {
		ray->setAbsorbtion(vec3(0, 0, 0));
		std::swap(n1, n2);
		normal = -1 * normal;
	}

	float n = n1 / n2;
	float k = 1 - n * n * (1 - cosTheta1 * cosTheta1);

	//if K , 0 then total internal reflection
	if (k < 0) {
		return 0;
	}

	//else return refraction direction
	return n * incidence + (n * cosTheta1 - sqrtf(k)) * normal;
}

float PathTracer::fresnelValue(vec3 incidence, vec3 normal, float refractiveIndex) {
	float cosTheta1 = normal.dot(incidence);
	float fresnelValue;
	if (cosTheta1 > 1) {
		cosTheta1 = 1;
	}
	else if (cosTheta1 < -1) {
		cosTheta1 = -1;
	}

	float n1 = 1;
	float n2 = refractiveIndex;
	if (cosTheta1 > 0) {
		swap(n1, n1);
	}
	float sinTheta2 = n1 / n2 * sqrtf(std::max(0.f, 1 - cosTheta1 * cosTheta1));

	// check for total internal reflection
	if (sinTheta2 >= 1) {
		fresnelValue = 1;
		return fresnelValue;
	}

	float cosTheta2 = sqrtf(max(0.f, 1 - sinTheta2 * sinTheta2));
	cosTheta1 = fabsf(cosTheta1);
	float Rs = ((n2 * cosTheta1) - (n1 * cosTheta2)) / ((n2 * cosTheta1) + (n1 * cosTheta2));
	float Rp = ((n1 * cosTheta1) - (n2 * cosTheta2)) / ((n1 * cosTheta1) + (n2 * cosTheta2));
	fresnelValue = ((Rs * Rs) + (Rp * Rp)) / 2;
	return fresnelValue;
}

vec3 PathTracer::sampleHemisphere(vec3 normal) {
	float rand1 = getrandom(&seed1, &seed2);
	float rand2 = getrandom(&seed1, &seed2);

	float sinTheta = sqrtf(1 - rand1 * rand1);
	float phi = DOUBLEPI * rand2;
	float x = sinTheta * cosf(phi);
	float z = sinTheta * sinf(phi);
	vec3 sampleDirection = vec3(x, rand1, z);
	vec3 Nb;
	vec3 Nt;

	//creating coordinate systen around the normal
	if (fabs(normal.x) > fabs(normal.y)) {
		float denom = 1 / sqrtf(normal.x * normal.x + normal.z * normal.z);
		Nt = vec3(normal.z, 0, -1.0f * normal.x) * denom;
	}
	else {
		float denom = 1 / sqrtf(normal.y * normal.y + normal.z * normal.z);
		Nt = vec3(0, -1.0f * normal.z, normal.y) * denom;
	}
	Nb = normal.cross(Nt);

	Nt = vec3(normal.y*Nb.z - normal.z*Nb.y, normal.z*Nb.x - normal.x*Nb.z, normal.x*Nb.y - normal.y*Nb.x);

	vec3 result = vec3(sampleDirection.x * Nb.x + sampleDirection.y * normal.x + sampleDirection.z * Nt.x, sampleDirection.x * Nb.y + sampleDirection.y * normal.y + sampleDirection.z * Nt.y, sampleDirection.x * Nb.z + sampleDirection.y * normal.z + sampleDirection.z * Nt.z);
	result.normalize();

	return result;
}

vec3 PathTracer::cosineWeightedSample(vec3 normal) {

	float rand1 = getrandom(&seed1, &seed2);
	float rand2 = getrandom(&seed1, &seed2);

	const float r = sqrtf(rand1);
	const float theta = DOUBLEPI * rand2;

	const float x = r * cosf(theta);
	const float z = r * sinf(theta);

	vec3 sampleDirection = vec3(x, sqrtf(max(0.0f, 1 - rand1)), z);
	sampleDirection.normalize();
	vec3 Nb;
	vec3 Nt;

	//createing coordinate systen around the normal
	if (fabs(normal.x) > fabs(normal.y)) {
		float denom = 1 / sqrtf(normal.x * normal.x + normal.z * normal.z);
		Nt = vec3(normal.z, 0, -1.0f * normal.x) * denom;
	}
	else {
		float denom = 1 / sqrtf(normal.y * normal.y + normal.z * normal.z);
		Nt = vec3(0, -1.0f * normal.z, normal.y) * denom;
	}
	Nb = normal.cross(Nt);

	Nt = vec3(normal.y*Nb.z - normal.z*Nb.y, normal.z*Nb.x - normal.x*Nb.z, normal.x*Nb.y - normal.y*Nb.x);

	vec3 result = vec3(sampleDirection.x * Nb.x + sampleDirection.y * normal.x + sampleDirection.z * Nt.x, sampleDirection.x * Nb.y + sampleDirection.y * normal.y + sampleDirection.z * Nt.y, sampleDirection.x * Nb.z + sampleDirection.y * normal.z + sampleDirection.z * Nt.z);
	result.normalize();
	return result;
}

vec4 PathTracer::randomLightAndPoint(Object** lights, int lightCount) {
	float rand1 = getrandom(&seed1, &seed2);
	float split = 0;

	int counter = 1;
	while (counter <= (float)lightCount) {
		split += (1 / (float)lightCount);
		//cout << split << " " << counter << " " << rand1 << endl;
		if (rand1 <= (split)) {
			vec3 point = 0;
			lights[counter - 1]->getRandomPoint(point, seed1, seed2);
			//cout << point.x << " " << point.y << " " << point.z << endl;
			//cout << "OUT" << endl;
			return vec4(point.x, point.y, point.z, counter - 1);
		}
		counter++;
	}

	return vec4(0, 0, 0, 0);
}

vec4 PathTracer::areaWeightedRandomLightAndPoint(Object** lights, int lightCount, float totalLightArea) {
	float rand1 = getrandom(&seed1, &seed2);
	float split = 0;

	int counter = 1;
	while (counter <= lightCount) {
		split += lights[counter - 1]->getArea();
		float cdf = split / totalLightArea;
		//cout << split << " " << counter << " " << rand1 << endl;
		if (cdf > rand1) {
			vec3 point = 0;
			lights[counter - 1]->getRandomPoint(point, seed1, seed2);
			//cout << point.x << " " << point.y << " " << point.z << endl;
			//cout << "OUT" << endl;
			return vec4(point.x, point.y, point.z, counter - 1);
		}
		counter++;
	}
	return vec4(0, 0, 0, 0);
}


void PathTracer::setBVH(BVH b) {
	this->bvh = b;
	this->useBVH = true;
}

