//
// Created by George Lorenzetti on 1/12/18.
//

#include "precomp.h"

TriangleMesh::TriangleMesh(int count, vector<vec3> vertices, vector<int> indices, vec3 color, Material material) {
	this->triangleCount = count;
	this->vertices = vertices;
	this->indices = indices;
	this->color = color;
	this->material = material;
	if (material.getMaterialType() != light) {
		this->intensity = 0.f;
	}
	else {
		this->intensity = 1.f;
	}
}

Intersection TriangleMesh::intersects(Ray *ray) {
	Intersection I = Intersection();
	I.t = -1;
	for (int i = 0; i < triangleCount; i++) {
		//cout << i << " " << 3*i+1 << " " << indices[3*i+2] << endl;
		Triangle triangle = Triangle(vertices[indices[3 * i]], vertices[indices[(3 * i) + 1]], vertices[indices[(3 * i) + 2]], color, material);
		Intersection I2 = triangle.intersects(ray);

		//check if hit and then compare current best with new triangle
		if (I2.t > -1) {
			if (I.t < 0 || (I.t > I2.t)) {
				I = I2;
			}
		}
	}
	//cout << "DONE" << endl;
	return I;
}

bool TriangleMesh::shadowRayIntersects(Ray ray) {
	for (int i = 0; i < triangleCount; i++) {
		Triangle triangle = Triangle(vertices[indices[(3 * i)]], vertices[indices[(3 * i) + 1]], vertices[indices[(3 * i) + 2]], color, material);
		if (triangle.shadowRayIntersects(ray)) {
			return true;
		}
	}
	return false;
}

Object** TriangleMesh::getTriangles() {
	Object** objects = new Object*[triangleCount];
	printf("triangle c %d", triangleCount);
	for (int i = 0; i < triangleCount; i++) {
		//cout << i << " " << 3*i+1 << " " << indices[3*i+2] << endl;
		objects[i] = new Triangle(vertices[indices[3 * i]], vertices[indices[(3 * i) + 1]], vertices[indices[(3 * i) + 2]], color, material);
	}

	return objects;
}

//for area lights
void TriangleMesh::getRandomPoint(vec3 &rp, unsigned int seed1, unsigned int seed2) {

}

vec3 TriangleMesh::getNormal(vec3 point) {
	return vec3(0.0f, 0.0f, 0.0f);
}

vec3 TriangleMesh::getColor() {
	return this->color;
}


float TriangleMesh::getArea() { return 0.f; }