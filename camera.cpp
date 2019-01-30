//
// Created by George Lorenzetti on 19/11/18.
//

#include "precomp.h"

Camera::Camera(){
    //initialize Camera transformation matrix as identity matrix
    cameraToWorldMatrix = mat4();
    Camera::cameraToWorldMatrix.cell[0] = 1;
    Camera::cameraToWorldMatrix.cell[5] = 1;
    Camera::cameraToWorldMatrix.cell[10] = 1;
    Camera::cameraToWorldMatrix.cell[15] = 1;

    rotationTheta = vec3(0,0,0);
}

float Camera::getFOV() { return fov; }
void Camera::adjustFOV(int f) {
    float step = 0.2f;
    fov += f*step;
}

mat4 Camera::getMatrix(){ return cameraToWorldMatrix; }
void Camera::setMatrix(mat4 matrix){ cameraToWorldMatrix = matrix; }

void Camera::rotate(char axis, float theta) {
    mat4 rotation;
    switch(axis){
        case 'x':
            rotation = rotation.rotatex(theta);
            cameraToWorldMatrix = rotation*cameraToWorldMatrix;
            break;
        case 'y':
            rotation = rotation.rotatey(theta);
            cameraToWorldMatrix = rotation*cameraToWorldMatrix;
            break;
        default:
            break;
    }
}

vec4 Camera::translate(vec3 d, float delta) {
    mat4 translate = mat4();
    translate.cell[12] = d.x*delta;
    translate.cell[13] = d.y*delta;
    translate.cell[11] = d.z*delta;

    vec4 temp = cameraPosition;
    cameraToWorldMatrix = translate * cameraToWorldMatrix;
    cameraPosition = cameraPosition*cameraToWorldMatrix;

    return (cameraPosition - temp);
   // cout << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z << endl;
}

float Camera::getSpeed(){
    return this->speed;
}
void Camera::setSpeed(float f){
    this->speed = f;
}