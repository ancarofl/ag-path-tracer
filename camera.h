//
// Created by George Lorenzetti on 19/11/18.
//

#ifndef TMPL8_2018_01_CAMERA_H
#define TMPL8_2018_01_CAMERA_H

#include "precomp.h"

class Camera {
public:
    Camera();

    float getFOV();
    void adjustFOV(int y);

    mat4 getMatrix();
    void setMatrix(mat4 matrix);

    float getSpeed();
    void setSpeed(float f);

    void rotate(char axis, float theta);
    vec4 translate(vec3 d, float delta);

private:
    mat4 cameraToWorldMatrix;
    vec4 cameraPosition = vec4(0,0,0,1);
    float fov = 90;
    float speed = 0.3f;
    vec3 rotationTheta;
};


#endif //TMPL8_2018_01_CAMERA_H
