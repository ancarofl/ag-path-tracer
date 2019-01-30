//
// Created by George Lorenzetti on 26/1/19.
//

#ifndef TMPL8_2018_01_WALL_H
#define TMPL8_2018_01_WALL_H

#include "precomp.h"

class Wall: public Object {
public:
    Wall(float z, float length, float depth, float Ox, float Oy, Material material);
    Wall(float z, float length, float depth, float Ox, float Oy, Material material, vec3 color);
    Wall(float x, float height, float depth, float Oy, float Oz, Material material, vec3 color, char axis);
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
    char axis = 'x';
    float x;
    float height;
    float depth;
    float offsetY;
    float offsetZ;
    vec3 color = vec3(1.0f, 1.0f, 1.0f);
    Material material;
};


#endif //TMPL8_2018_01_WALL_H
