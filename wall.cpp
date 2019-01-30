//
// Created by George Lorenzetti on 26/1/19.
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

Wall::Wall(float x, float height, float depth, float Oy, float Oz, Material material) {
    this->x = x;
    this->height = height;
    this->depth = depth;
    this->offsetY = Oy;
    this->offsetZ = Oz;
    this->material = material;
    bounds.bounds[1].x = x;
    bounds.bounds[0].x = x;
    bounds.bounds[1].y = offsetY + height;
    bounds.bounds[0].y = offsetY - height;
    bounds.bounds[1].z = offsetZ + depth;
    bounds.bounds[0].z = offsetZ - depth;
    bounds.calcCentre();
}

Wall::Wall(float x, float height, float depth, float Oy, float Oz, Material material, vec3 color) {
    this->x = x;
    this->height = height;
    this->depth = depth;
    this->offsetY = Oy;
    this->offsetZ = Oz;
    this->material = material;
    bounds.bounds[1].x = x;
    bounds.bounds[0].x = x;
    bounds.bounds[1].y = offsetY + height;
    bounds.bounds[0].y = offsetY - height;
    bounds.bounds[1].z = offsetZ + depth;
    bounds.bounds[0].z = offsetZ - depth;
    bounds.calcCentre();
    this->color = color;
}

Wall::Wall(float x, float height, float depth, float Oy, float Oz, Material material, vec3 color, char axis) {
    this->x = x;
    this->height = height;
    this->depth = depth;
    this->offsetY = Oy;
    this->offsetZ = Oz;
    this->material = material;
    this->axis = axis;
    if(axis == 'x') {
        bounds.bounds[1].x = x;
        bounds.bounds[0].x = x;
        bounds.bounds[1].y = offsetY + height;
        bounds.bounds[0].y = offsetY - height;
        bounds.bounds[1].z = offsetZ + depth;
        bounds.bounds[0].z = offsetZ - depth;
        bounds.calcCentre();
    }else{
        bounds.bounds[1].x = offsetZ + depth;
        bounds.bounds[0].x = offsetZ - depth;
        bounds.bounds[1].y = offsetY + height;
        bounds.bounds[0].y = offsetY - height;
        bounds.bounds[1].z = x;
        bounds.bounds[0].z = x;
        bounds.calcCentre();
    }
    this->color = color;
}

Intersection Wall::intersects(Ray *ray) {
    Intersection I;
    I.t = -1;

    vec3 normal;
    if(axis == 'x') {
        normal = vec3(-1, 0, 0);
    }else{
        normal = vec3(0,0,-1);
    }

    if(ray->getDirection().dot(normal) > 0){
        normal *= -1.0f;
    }

    float denom = normal.dot(ray->getDirection());
    if (abs(denom) > 0.01f) // your favorite epsilon
    {
        float t;
        if(axis == 'x') {
            t = (vec3(this->x, 0, this->offsetZ) - ray->getOrigin()).dot(normal) / denom;
        }else{
            t = (vec3(this->offsetZ, 0, this->x) - ray->getOrigin()).dot(normal) / denom;
        }

        if (t < 0) {
            return I;
        }

        vec3 point = ray->getOrigin() + (t * ray->getDirection());

        float pCompare;

        axis == 'x' ? pCompare = point.z : pCompare = point.x;
        if (point.y < offsetY - height / 2 || point.y > offsetY + height / 2 || pCompare < offsetZ - depth / 2 || pCompare > offsetZ + depth / 2) {
            return I;
        }

        I.t = t;
        I.point = point;
        I.normal = normal;
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
        if (I.t > 0.01f)
            return I; // you might want to allow an epsilon here too
    }
    I.t = -1;
    return I;
}

bool Wall::shadowRayIntersects(Ray ray) {
    vec3 normal;
    if(axis == 'x') {
        normal = vec3(-1, 0, 0);
    }else{
        normal = vec3(0,0,-1);
    }

    if(ray.getDirection().dot(normal) > 0){
        normal *= -1.0f;
    }

    float denom = normal.dot(ray.getDirection());
    if (abs(denom) > 0.01f) // your favorite epsilon
    {
        float t;
        if(axis == 'x') {
            t = (vec3(this->x, 0, this->offsetZ) - ray.getOrigin()).dot(normal) / denom;
        }else{
            t = (vec3(this->offsetZ, 0, this->x) - ray.getOrigin()).dot(normal) / denom;
        }

        if (t < 0) {
            return false;
        }

        vec3 point = ray.getOrigin() + (t * ray.getDirection());

        float pCompare;

        axis == 'x' ? pCompare = point.z : pCompare = point.x;
        if (point.y < offsetY - height / 2 || point.y > offsetY + height / 2 || pCompare < offsetZ - depth / 2 || pCompare > offsetZ + depth / 2) {
            return false;
        }

        if (t > 0.01f) return true; // you might want to allow an epsilon here too

    }
    return false;

}

/*for pt*/
void Wall::getRandomPoint(vec3 &rp, unsigned int seed1, unsigned int seed2) {
    float randomVal1 = getrandom(&seed1, &seed2);
    randomVal1 *= height;

    float randomVal2 = getrandom(&seed1, &seed2);
    randomVal2 *= depth;

    rp = vec3(x, randomVal1 + offsetY, randomVal2 + offsetZ);
}

vec3 Wall::getNormal(vec3 point) {
    return vec3(1, 0, 0);
}

vec3 Wall::getColor() {
    return this->color;
}


float Wall::getArea() {
    return (height*depth);
}