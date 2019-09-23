#ifndef _RAY_HIT_H
#define _RAY_HIT_H

#include "libs/Matrix.h"

#include <string>

struct rayHit
{
    float intersectionTime;
    Vec3 intersectionPoint;
    Vec3 surfaceNormal;
    std::string materialID;
    //Material
};

struct hitBundle
{
    rayHit records[4];

    rayHit& operator[](int index)
    {
        return records[index];
    }
};

#endif
