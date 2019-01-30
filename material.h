#ifndef TMPL8_2018_01_MATERIAL_H
#define TMPL8_2018_01_MATERIAL_H

#include "precomp.h"

enum MaterialType {diffuse, textured, reflective, snell, fresnel, beer, light, phong};

class Material  {
public:

	int phongFactor = 50;

	float emission = 0;

    MaterialType getMaterialType() {
        return mt;
    };
    void setMaterialType(MaterialType mt) { this->mt = mt; };
    Material(MaterialType mt);
    Material(MaterialType mt, int texture);
    Material(MaterialType mt, float v);
    Material(MaterialType mt, float v, vec3 a);
    Material(MaterialType mt, bool tri);

    Material() = default;

    float getSpecularity();
    void setSpecularity(float sp);

    float getIndex();
    void setIndex(float index);

    vec3 getAbsorbtion();
    void setAbsorbtion(vec3 a);

    int getTexture();
    void setTexture(int index);

	void setEmission(float e);
	void setPhongFactor(int p);

    bool isTriangle = false;

private:
    MaterialType mt;
    float specularity;
    float refractiveIndex;
    vec3 absorbtion;
    int textureIndex;
};


#endif //TMPL8_2018_01_MATERIAL_H
