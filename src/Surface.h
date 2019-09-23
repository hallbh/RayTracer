#ifndef _SURFACE_H
#define _SURFACE_H

#include "BoundingBox.h"
#include "Ray.h"
#include "rayHit.h"

#include <string>

class Surface
{
protected:
    std::string materialName;

public:
    Surface(std::string materialID);
    virtual ~Surface() = default;

    virtual bool hit(Ray ray, float startTime, rayHit &record) = 0;

    virtual Vec3 getCentroid() = 0;
    virtual BoundingBox getBoundingBox() = 0;
};

Surface::Surface(std::string materialID)
    : materialName(materialID)
{}

#endif 
