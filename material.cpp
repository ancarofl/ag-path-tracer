//
// Created by Anca Negulescu on 30/11/18.
//

#include "precomp.h"

Material::Material(MaterialType mt){
    this->mt = mt;
    specularity = 0;
    refractiveIndex = 0;
    if(mt == textured){
        this->textureIndex = 0;
    }
    if(mt == snell || mt == fresnel){
        this->refractiveIndex = 1.52f;
    }
    if(mt == reflective){
        specularity = 1;
    }
	if (mt == light) {
		emission = 25.0f;
	}
}

void Material::setEmission(float e) {
	if (mt == light) {
		emission = e;
	}
}

void Material::setPhongFactor(int p) {
	if (mt == phong) {
		phongFactor = p;
	}
}

Material::Material(MaterialType mt, int texture){
    this->mt = mt;
    specularity = 0;
    refractiveIndex = 0;
    if(mt == textured){
        this->textureIndex = texture;
    }
    if(mt == snell || mt == fresnel){
        this->refractiveIndex = 1.52f;
    }
    if(mt == reflective){
        specularity = 1;
    }
}

Material::Material(MaterialType mt, float v){
    this->mt = mt;
    specularity = 0;
    refractiveIndex = 0;
    if(mt == textured){
        this->textureIndex = 0;
    }
    if(mt == snell || fresnel){
        this->refractiveIndex = v;
    }
    if(mt == reflective){
        specularity = v;
    }
}

Material::Material(MaterialType mt, float v, vec3 a){
    this->mt = mt;
    specularity = 0;
    refractiveIndex = 0;
    if(mt == textured){
        this->textureIndex = 0;
    }
    if(mt == snell || fresnel || beer){
        this->refractiveIndex = v;
    }
    if(mt == reflective){
        specularity = v;
    }

    this->absorbtion = a;
}

Material::Material(MaterialType mt, bool tri){
    this->mt = mt;
    specularity = 0;
    refractiveIndex = 0;
    if(mt == textured){
        this->textureIndex = 0;
    }
    if(mt == snell || mt == fresnel){
        this->refractiveIndex = 1.52f;
    }
    if(mt == reflective){
        specularity = 1;
    }
    if (mt == light) {
        emission = 8;
    }

    isTriangle = true;
}

float Material::getIndex() { return refractiveIndex; }
void Material::setIndex(float index) { this->refractiveIndex = index ;}

float Material::getSpecularity() { return specularity; }
void Material::setSpecularity(float sp){ this->specularity = sp; }

vec3 Material::getAbsorbtion() { return absorbtion; }
void Material::setAbsorbtion(vec3 a){ this->absorbtion = a; }

int Material::getTexture() { return textureIndex; }
void Material::setTexture(int index) { this->textureIndex = index ;}