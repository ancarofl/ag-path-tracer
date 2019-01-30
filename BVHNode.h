//
// Created by George Lorenzetti on 15/12/18.
//

#ifndef TMPL8_2018_01_BVHNODE_H
#define TMPL8_2018_01_BVHNODE_H

#include "precomp.h"

class BVHNode {
public:
    void subdivide(Object** objects, int* indices);
    int partition(Object** objects, int* indices);
    void calculateBounds(Object** objects, int first, int* indices, int count);
    Intersection traverse(Ray ray, Object **objects, int* indices, Material &mt);
    bool shadowTraverse(Ray ray, Object **objects, int* indices);

    AABB getBounds();
    void setBounds(AABB b);

    bool getIsLeaf();
    void setIsLeaf(bool b);

    BVHNode* getLeft();
    void setLeft(BVHNode* b);

    BVHNode* getRight();
    void setRight(BVHNode* b);

    int getFirst();
    void setFirst(int b);

    int getCount();
    void setCount(int b);

    void setShowBVH(bool t);
    vec3 backgroundColor =  vec3(12/255.0f, 164/255.0f, 213/255.f);

private:
    AABB bounds;
    bool isLeaf;
    BVHNode* left;
    BVHNode* right;
    int first;
    int count;
};


#endif //TMPL8_2018_01_BVHNODE_H
