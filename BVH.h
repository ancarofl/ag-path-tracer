//
// Created by George Lorenzetti on 15/12/18.
//

#ifndef TMPL8_2018_01_BVH_H
#define TMPL8_2018_01_BVH_H

#include "precomp.h"

class BVH {
public:
    void constructBVH(Object** objects, int objectCount);
    BVHNode getRoot();
    int* getIndices();
private:
    BVHNode root;
    int* indices;
    BVHNode* pool;
};


#endif //TMPL8_2018_01_BVH_H
