//
// Created by George Lorenzetti on 15/12/18.
//

#include "precomp.h"

//In progress
void BVH::constructBVH(Object** objects, int objectCount){
    //create an array of indices for the object array. Each BVHNode corresponds to a sublist in this array
    indices = new int[objectCount];
    for(int i = 0; i < objectCount; i++){
        indices[i] = i;
    }
    //create the root node of the tree
    this->root = BVHNode();
    this->root.setFirst(0);
    this->root.setCount(objectCount);
    this->root.calculateBounds(objects, this->root.getFirst(), indices, this->root.getCount());

    //subdivide to create the child nodes recursively
    this->root.subdivide(objects, indices);
}

BVHNode BVH::getRoot(){
    return this->root;
}

int* BVH::getIndices(){
    return this->indices;
}
