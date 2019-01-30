//
// Created by George Lorenzetti on 15/12/18.
//

#include "precomp.h"

//subdivide node to build BVH
void BVHNode::subdivide(Object** objects, int* indices) {

    //base case, this is a leaf node if there are less than 3 objects inside
    if(this->count < 3){
        this->setIsLeaf(true);
        for(int i = first; i < first+count; i++){
        }
        return;
    }

    this->left = new BVHNode();
    this->right = new BVHNode();

    //calculate partition
    int split = partition(objects, indices);
    //if no split was made because child nodes do not improve cost then make this a leaf
    if(split == -1){
        this->setIsLeaf(true);
        for(int i = first; i < first+count; i++){
        }
        return;
    }
    if(split == first){
        return;
    }
    //else subdivide
    this->left->setFirst(first);
    this->left->setCount(split - first);
    this->left->calculateBounds(objects, this->left->getFirst(), indices, this->left->getCount());

    this->right->setFirst(split);
    this->right->setCount(count - this->left->getCount());
    this->right->calculateBounds(objects, this->right->getFirst(), indices, this->right->getCount());

    //recursive calls to partition child nodes
    if(left->count > 0) {
        this->left->subdivide(objects, indices);
    }else{
        this->left->setIsLeaf(true);
    }
    if(right->count > 0){
        this->right->subdivide(objects, indices);
    }else{
        this->right->setIsLeaf(true);
    }

    this->setIsLeaf(false);

}

//TO IMPLEMENT
int BVHNode::partition(Object** objects, int* indices){
    /*--FINDING SPLIT AXIS--*/
    //Using the Arbitrary Acyclic Algorithm from: http://graphicsinterface.org/wp-content/uploads/gi1989-22.pdf
    //spatial medians
    float xSpatialMedian = (this->bounds.bounds[0].x + this->bounds.bounds[1].x)/2.0f;
    float ySpatialMedian = (this->bounds.bounds[0].y + this->bounds.bounds[1].y)/2.0f;
    float zSpatialMedian = (this->bounds.bounds[0].z + this->bounds.bounds[1].z)/2.0f;

    //object medians
    float xObjectMedian;
    float yObjectMedian;
    float zObjectMedian;

	float* xVals = new float[count];
	float* yVals = new float[count];
	float* zVals = new float[count];

    for(int i = first; i < (first+count); i++){
        xVals[i-first] = objects[indices[i]]->bounds.centrePoint.x;
        yVals[i-first] = objects[indices[i]]->bounds.centrePoint.y;
        zVals[i-first] = objects[indices[i]]->bounds.centrePoint.z;
    }

    sort(xVals, xVals+count);
    sort(yVals, yVals+count);
    sort(zVals, zVals+count);

    xObjectMedian = xVals[count/2];
    yObjectMedian = yVals[count/2];
    zObjectMedian = zVals[count/2];

    //choose 9 equally spaced possibilities on each axis and check cost.
    float height = this->bounds.bounds[1].y - this->bounds.bounds[0].y;
    float width = this->bounds.bounds[1].x - this->bounds.bounds[0].x;
    float depth = this->bounds.bounds[1].z - this->bounds.bounds[0].z;

    float SA = 2*(height*width) + 2*(height*depth) + 2*(width*depth);
    float parentCost = SA*this->count;
    float splitCost = parentCost;
    char splitAxis = 'x';
    float splitPosition = xSpatialMedian;
    //x axis
    float min = fminf(xObjectMedian, xSpatialMedian);
    float max = fmaxf(xObjectMedian, xSpatialMedian);
    float step = (max-min)/9.0f;
    for(int i = 0; i < 9; i++){
        float split = (min+(step*i));
        float tempCost;

        float widthL = split - this->bounds.bounds[0].x;
        float widthR = this->bounds.bounds[1].x - split;

        float SALeft = 2*(height*widthL) + 2*(height*depth) + 2*(widthL*depth);
        float SARight = 2*(height*widthR) + 2*(height*depth) + 2*(widthR*depth);

        int n = 0;
        while(xVals[n] < split){
            n++;
        }

        tempCost = (SALeft*(float)n) + (SARight*((float)count - (float)n));
        if(tempCost < splitCost){
            splitCost = tempCost;
            splitAxis = 'x';
            splitPosition = split;
        }
    }

    //y axis
    min = fminf(yObjectMedian, ySpatialMedian);
    max = fmaxf(yObjectMedian, ySpatialMedian);
    step = (max-min)/9.0f;
    for(int i = 0; i < 9; i++){
        float split = (min+(step*i));
        float tempCost;

        float heightL = split - this->bounds.bounds[0].y;
        float heightR = this->bounds.bounds[1].y - split;

        float SALeft = 2*(heightL*width) + 2*(heightL*depth) + 2*(width*depth);
        float SARight = 2*(heightR*width) + 2*(heightR*depth) + 2*(width*depth);

        int n = 0;
        while(yVals[n] < split){
            n++;
        }

        tempCost = (SALeft*(float)n) + (SARight*((float)count - (float)n));
        if(tempCost < splitCost){
            splitCost = tempCost;
            splitAxis = 'y';
            splitPosition = split;
        }
    }

    //z axis
    min = fminf(zObjectMedian, zSpatialMedian);
    max = fmaxf(zObjectMedian, zSpatialMedian);
    step = (max-min)/9.0f;
    for(int i = 0; i < 9; i++){
        float split = (min+(step*i));
        float tempCost;

        float depthL = split - this->bounds.bounds[0].z;
        float depthR = this->bounds.bounds[1].z - split;

        float SALeft = 2*(height*width) + 2*(height*depthL) + 2*(width*depthL);
        float SARight = 2*(height*width) + 2*(height*depthR) + 2*(width*depthR);

        int n = 0;
        while(zVals[n] < split){
            n++;
        }

        tempCost = (SALeft*(float)n) + (SARight*((float)count - (float)n));
        if(tempCost < splitCost){
            splitCost = tempCost;
            splitAxis = 'z';
            splitPosition = split;
        }
    }

    //if best cost is the same at the parent cost then don't split and just have a leaf node
    if(splitCost == parentCost){
        return -1;
    }
    vector<int> lessEqual;
    vector<int> more;
    //sort into 2 arrays of object indices on the left and right of partition
    for(int i = first; i < (first+count); i++){
        switch(splitAxis){
            case 'x':
                if(objects[indices[i]]->bounds.centrePoint.x <= splitPosition){
                    lessEqual.push_back(indices[i]);
                }else{
                    more.push_back(indices[i]);
                }
                break;
            case 'y':
                if(objects[indices[i]]->bounds.centrePoint.y <= splitPosition){
                    lessEqual.push_back(indices[i]);
                }else{
                    more.push_back(indices[i]);
                }
                break;
            case 'z':
                if(objects[indices[i]]->bounds.centrePoint.z <= splitPosition){
                    lessEqual.push_back(indices[i]);
                }else{
                    more.push_back(indices[i]);
                }
                break;
        }
    }

    //if next partition is same as current partition
    if(lessEqual.size() == 0 || more.size() == 0){
        return -1;
    }

    //rearrange indices according to partition
    int partitionPoint = first+count-1;
    for(int i = 0; i < lessEqual.size(); i++){
        indices[first+i] = lessEqual[i];
    }
    for(int i = 0; i < more.size(); i++){
        indices[first+lessEqual.size()+i] = more[i];
    }
    //return the first index for the right child as the point of partition
    return first+lessEqual.size();
}

//TO IMPLEMENT
void BVHNode::calculateBounds(Object **objects, int first, int* indices, int count) {
    AABB bnds;

    bnds.bounds[1].x = objects[indices[first]]->bounds.bounds[1].x;
    bnds.bounds[0].x = objects[indices[first]]->bounds.bounds[0].x;
    bnds.bounds[1].y = objects[indices[first]]->bounds.bounds[1].y;
    bnds.bounds[0].y = objects[indices[first]]->bounds.bounds[0].y;
    bnds.bounds[1].z = objects[indices[first]]->bounds.bounds[1].z;
    bnds.bounds[0].z = objects[indices[first]]->bounds.bounds[0].z;

    for(int i = first+1; i < (first+count); i++) {
        if (bnds.bounds[1].x < objects[indices[i]]->bounds.bounds[1].x) {
            bnds.bounds[1].x = objects[indices[i]]->bounds.bounds[1].x;
        }

        if (bnds.bounds[0].x > objects[indices[i]]->bounds.bounds[0].x) {
            bnds.bounds[0].x = objects[indices[i]]->bounds.bounds[0].x;
        }

        if (bnds.bounds[1].y < objects[indices[i]]->bounds.bounds[1].y) {
            bnds.bounds[1].y = objects[indices[i]]->bounds.bounds[1].y;
        }

        if (bnds.bounds[0].y > objects[indices[i]]->bounds.bounds[0].y) {
            bnds.bounds[0].y = objects[indices[i]]->bounds.bounds[0].y;
        }

        if (bnds.bounds[1].z < objects[indices[i]]->bounds.bounds[1].z) {
            bnds.bounds[1].z = objects[indices[i]]->bounds.bounds[1].z;
        }

        if (bnds.bounds[0].z > objects[indices[i]]->bounds.bounds[0].z) {
            bnds.bounds[0].z = objects[indices[i]]->bounds.bounds[0].z;
        }
    }

    bnds.calcCentre();
    this->bounds = bnds;
}

Intersection BVHNode::traverse(Ray ray, Object **objects, int* indices, Material &mt){
    Intersection I;

    //if does not intersect node
    if(!bounds.intersect(ray)){
        I.t = -1;
        I.color = backgroundColor;
        return I;
    }

    if(this->isLeaf){
        I.t = -1;
        for (int i = first; i < first+count; i++) {
            Intersection newIntersection = objects[indices[i]]->intersects(&ray);
            if (newIntersection.t > 0 && (I.t == -1 || I.t > newIntersection.t)) {
                I = newIntersection;
                mt = objects[indices[i]]->getMaterial();
            }
        }
        if (I.t < 0) {
            I.color = backgroundColor;
        }
        return I;

    }else{
        Intersection leftI = this->left->traverse(ray, objects, indices, mt);
        Intersection rightI = this->right->traverse(ray, objects, indices, mt);

        if(leftI.t < 0){
            return rightI;
        }
        if(rightI.t < 0){
            return leftI;
        }
        if(leftI.t < rightI.t){
            return leftI;
        }else{
            return rightI;
        }
    }
}

bool BVHNode::shadowTraverse(Ray ray, Object **objects, int *indices){

    //if does not intersect node
    if(!bounds.intersect(ray)){
        return false;
    }

    if(this->isLeaf){
        for (int i = first; i < first+count; i++) {
            bool newIntersection = objects[indices[i]]->shadowRayIntersects(ray);
            if (newIntersection) {
                return true;
            }
        }
        return false;
    }else{
        bool leftI = this->left->shadowTraverse(ray, objects, indices);
        bool rightI = this->right->shadowTraverse(ray, objects, indices);

        if(leftI || rightI){
            return true;
        }
        return false;
    }
}

//getters and setters
AABB BVHNode::getBounds(){
    return this->bounds;
}
void BVHNode::setBounds(AABB b){
    this->bounds = b;
}

bool BVHNode::getIsLeaf(){
    return this->isLeaf;
}
void BVHNode::setIsLeaf(bool b){
    this->isLeaf = b;
}

BVHNode* BVHNode::getLeft(){
    return this->left;
}
void BVHNode::setLeft(BVHNode* b){
    this->left = b;
}

BVHNode* BVHNode::getRight(){
    return this->right;
}
void BVHNode::setRight(BVHNode* b){
    this->right = b;
}

int BVHNode::getFirst(){
    return this->first;
}
void BVHNode::setFirst(int b){
    this->first = b;
}

int BVHNode::getCount(){
    return this->count;
}
void BVHNode::setCount(int b){
    this->count = b;
}