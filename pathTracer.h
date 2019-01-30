//
// Created by George Lorenzetti on 13/1/19.
//

#ifndef TMPL8_2018_01_PATHTRACER_H
#define TMPL8_2018_01_PATHTRACER_H

#include "precomp.h"

class PathTracer {
public:
	void calculateRays(Surface* screen, Camera camera);
	Intersection nearestIntersection(Ray ray, Object** objects, int objectCount, Material &mt);
	
	vec3 directIllumination(Object** objects, int objectCount, Object** areaLights, int areaLightCount, Intersection I, vec3 BRDF, float totalLightArea);
	vec3 indirectIllumination(Object** objects, int objectCount, Object** areaLights, int lightCount, Ray ray);
	vec3 sampleHemisphere(vec3 normal);
	vec4 randomLightAndPoint(Object** lights, int lightCount);
	vec4 areaWeightedRandomLightAndPoint(Object** lights, int lightCount, float totalLightArea);
	vec3 cosineWeightedSample(vec3 normal);
    bool testShadow(Ray sray, Object** objects, int objectCount, Object** areaLights, int areaLightCount);
    vec3 refract(vec3 incidence, vec3 normal, float refractiveIndex, Ray* ray);
	float fresnelValue(vec3 incidence, vec3 normal, float refractiveIndex);
	void setBVH(BVH b);

	// trace functions
	vec3 trace(Object** objects, int objectCount, Object** areaLights, int areaLightCount, float totalLightArea, Ray ray, int iterations, vec3 &result, bool lastSpecular);
	
	vec4 topLeft;
	vec4 topRight;
	vec4 bottomLeft;
	vec3 horizontalStep;
	vec3 verticalStep;
    unsigned int seed1;
    unsigned int seed2;
    bool useNEE = false;
    bool useIS = false;
	bool useRR = false;
	bool useDOF = false;
	bool hasSKYBOX = false;
	bool useFILTERING = false;
private:
	Ray** rays;
	BVH bvh;
	bool useBVH = false;

};


#endif //TMPL8_2018_01_PATHTRACER_H
