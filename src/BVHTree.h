#ifndef _BVH_TREE_H
#define _BVH_TREE_H

// #define RENDER_LEAF_BBOX

#include "BoundingBox.h"
#include "Ray.h"
#include "rayHit.h"
#include "Surface.h"

#include "libs/Matrix.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string.h>
#include <utility>
#include <vector>

struct alignas(32) BVHNode
{
    float boundingBox[6];
    union{
        struct{
            char isInnerNode;
            int childrenOffset;
        };
        Surface* surf;
    };
};

static_assert(offsetof(BVHNode, surf) == offsetof(BVHNode, isInnerNode), "Byte alignment in BVHNode is off, might need to adjust for endianness!");

class BVHTree
{
private:
    unsigned int nextFreeNode;
    unsigned int allocatedSize;
    BVHNode *nodes;

public:
    BVHTree(std::vector<Surface*>&);
    ~BVHTree();

    bool hit(Ray ray, float startTime, float endTime, rayHit &record);
    void hit(rayBundle rays, float startTime, float endTime, hitBundle &records);

private:
    void build(std::vector<Surface*> &surfaces, int nodeIndex);
    bool hitNodeList(Ray ray, float startTime, rayHit &record, int nodeOfInterest);
    void hitNodeList(rayBundle rays, float startTime, hitBundle &records, int nodeOfInterest);
};

BVHTree::BVHTree(std::vector<Surface*> &surfaces)
{
	//terrible heuristic that *probably* makes enough space
	//may need coercing to align with 32 byte boundaries
    this->nodes = new BVHNode[surfaces.size() * 4];
    this->allocatedSize = surfaces.size() * 4;

    this->nextFreeNode = 1;

    this->build(surfaces, 0);
}

BVHTree::~BVHTree()
{
    for (size_t i = 0; i < this->nextFreeNode; i++)
    {
        if (this->nodes[i].isInnerNode != 1)
            delete this->nodes[i].surf;
    }

    delete[] this->nodes;
}

bool BVHTree::hit(Ray ray, float startTime, float endTime, rayHit &record)
{
    return this->hitNodeList(ray, startTime, record, 0);
}

void BVHTree::hit(rayBundle rays, float startTime, float endTime, hitBundle &records)
{
    this->hitNodeList(rays, startTime, records, 0);
}

void BVHTree::build(std::vector<Surface*> &surfaces, int nodeIndex)
{
    BVHNode &thisNode = this->nodes[nodeIndex];
    memcpy(thisNode.boundingBox, surfaces[0]->getBoundingBox().minMax, sizeof(float) * 6);

    Vec3 centroid = surfaces[0]->getCentroid();

    for (size_t i = 1; i < surfaces.size(); i++)
    {
        BoundingBox bounds = surfaces[i]->getBoundingBox();
        for (int j = 0; j < 3; j++)
        {
            thisNode.boundingBox[j] = std::min(thisNode.boundingBox[j], bounds.minMax[j]);
            thisNode.boundingBox[j+3] = std::max(thisNode.boundingBox[j+3], bounds.minMax[j+3]);
        }

        centroid += surfaces[i]->getCentroid();
    }

    if (surfaces.size() == 1)
    {
        thisNode.surf = surfaces[0];
        return;
    }

    centroid = centroid / surfaces.size();

    float maxDimWidth = 0;
    int maxDim;
    
    for (int i = 0; i < 3; i++)
    {
        if (thisNode.boundingBox[i+3] - thisNode.boundingBox[i] > maxDimWidth)
        {
            maxDimWidth = thisNode.boundingBox[i+3] - thisNode.boundingBox[i];
            maxDim = i;
        }
    }

    float splitPoint = (thisNode.boundingBox[maxDim+3] + thisNode.boundingBox[maxDim]) / 2;
    splitPoint = centroid[maxDim];

    std::vector<Surface*> left, right;

    for (auto *surf : surfaces)
    {
        if (surf->getCentroid()[maxDim] < splitPoint)
        {
            left.push_back(surf);
        }
        else
        {
            right.push_back(surf);
        }
    }

    if (left.size() == 0)
    {
        left.push_back(right.back());
        right.pop_back();
    }
    if (right.size() == 0)
    {
        right.push_back(left.back());
        left.pop_back();
    }

    int claimedNodesIndex = this->nextFreeNode;
    this->nextFreeNode += 2;

    thisNode.isInnerNode = 1;
    thisNode.childrenOffset = claimedNodesIndex;

    if (this->nextFreeNode > this->allocatedSize)
    {
        throw new std::length_error("Too many things to fit in BVHTree!");
    }

    this->build(left, claimedNodesIndex);
    this->build(right, claimedNodesIndex + 1);
}

bool BVHTree::hitNodeList(Ray ray, float startTime, rayHit &record, int nodeOfInterest)
{
    BVHNode &thisNode = this->nodes[nodeOfInterest];

    bool hitSurface = false;

    rayHit newInfo = record;

    hitSurface = BoundingBox::hit(thisNode.boundingBox, ray, startTime, newInfo);

    if (!hitSurface)
        return false;

#ifdef RENDER_LEAF_BBOX
    if (!(thisNode.isInnerNode == 1))
    {
        record = newInfo;
        return hitSurface;
    }
#endif

    hitSurface = false;

    if (thisNode.isInnerNode == 1)
    {
        hitSurface = this->hitNodeList(ray, startTime, record, thisNode.childrenOffset);
        hitSurface = this->hitNodeList(ray, startTime, record, thisNode.childrenOffset + 1) || hitSurface;
    }
    else
    {
        hitSurface = thisNode.surf->hit(ray, startTime, record);
    }
    
    return hitSurface;
}

void BVHTree::hitNodeList(rayBundle rays, float startTime, hitBundle &records, int nodeOfInterest)
{
    BVHNode &thisNode = this->nodes[nodeOfInterest];

    bool hitSurface = false;

    bool mask[4];
    for (int i = 0; i < 4; i++) mask[i] = true;

    hitBundle newInfo = records;

    BoundingBox::hit(thisNode.boundingBox, rays, startTime, newInfo, mask);

    for (int i = 0; i < 4; i++)
    {
        hitSurface |= mask[i];
    }

    if (!hitSurface)
        return;

#ifdef RENDER_LEAF_BBOX
    if (!(thisNode.isInnerNode == 1))
    {
        records = newInfo;
        return;
    }
#endif

    if (thisNode.isInnerNode == 1)
    {
        this->hitNodeList(rays, startTime, records, thisNode.childrenOffset);
        this->hitNodeList(rays, startTime, records, thisNode.childrenOffset + 1);
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            thisNode.surf->hit(rays[i], startTime, records[i]);
        }
    }
}

#endif
