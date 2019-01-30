//
// Created by George Lorenzetti on 18/12/18.
//

#include "precomp.h"

void AABB::calcCentre(){
    centrePoint = vec3((bounds[0].x+bounds[1].x)/2.0f, (bounds[0].y+bounds[1].y)/2.0f, (bounds[0].z+bounds[1].z)/2.0f);
}

//AABB ray intersection using the slabs algorithm
bool AABB::intersect(Ray r) {
//	float tmin;
//	float tmax;
//	float tymin;
//	float tymax;
//	float tzmin;
//	float tzmax;
//
//	float ox = r.getOrigin().x;
//	float oy = r.getOrigin().y;
//	float ix = r.getInverse().x;
//	float iy = r.getInverse().y;
//
//	int sx = r.getSign(0);
//	int sy = r.getSign(1);
//
//	tmin = (bounds[sx].x - ox) * ix;
//	tmax = (bounds[1 - sx].x - ox)  * ix;
//	tymin = (bounds[sy].y - oy) *  iy;
//	tymax = (bounds[1 - sy].y - oy) * iy;
//
//	if ((tmin > tymax) || (tymin > tmax))
//		return false;
//	if (tymin > tmin)
//		tmin = tymin;
//	if (tymax < tmax)
//		tmax = tymax;
//
//	float oz = r.getOrigin().z;
//	float iz = r.getInverse().z;
//
//	int sz = r.getSign(2);
//
//	tzmin = (bounds[sz].z - oz) * iz;
//	tzmax = (bounds[1 - sz].z - oz) * iz;
//
//	if ((tmin > tzmax) || (tzmin > tmax))
//		return false;
//	if (tzmin > tmin)
//		tmin = tzmin;
//	if (tzmax < tmax)
//		tmax = tzmax;
//
//	return tmin > 0 || tmax > 0;
//

	//previous code
    float tmin;
    float tmax;
    float tymin;
    float tymax;
    float tzmin;
    float tzmax;

    tmin = (bounds[r.getSign(0)].x - r.getOrigin().x) * r.getInverse().x;
    tmax = (bounds[1-r.getSign(0)].x - r.getOrigin().x) * r.getInverse().x;
    tymin = (bounds[r.getSign(1)].y - r.getOrigin().y) * r.getInverse().y;
    tymax = (bounds[1-r.getSign(1)].y - r.getOrigin().y) * r.getInverse().y;

    if ((tmin > tymax) || (tymin > tmax))
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (bounds[r.getSign(2)].z - r.getOrigin().z) * r.getInverse().z;
    tzmax = (bounds[1-r.getSign(2)].z - r.getOrigin().z) * r.getInverse().z;

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    float t = tmin;
    if(t < 0){
        t = tmax;
        if(t < 0){
            return false;
        }
    }
    return true;
}