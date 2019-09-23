#ifndef _RAY_H
#define _RAY_H

class RayGenerator;

#include "libs/Matrix.h"

class Ray
{
private:
    Vec3 origin;
    Vec3 direction;
    Vec3 invDir;

public:
    Ray() = default;
    Ray(Vec3 origin, Vec3 direction);

    const Vec3 getDirection() const;
    const Vec3 getInverseDirection() const;
    Vec3 positionAtTime(float t);

    friend class RayGenerator;
};

Ray::Ray(Vec3 origin, Vec3 direction)
    : origin(origin)
{
    this->direction = Mat::normalize(direction);
    this->invDir = 1.0f / this->direction;
}

const Vec3 Ray::getDirection() const 
{
    return this->direction;
}

const Vec3 Ray::getInverseDirection() const
{
    return this->invDir;
}

Vec3 Ray::positionAtTime(float t)
{
    return this->origin + this->direction * t;
}

struct rayBundle
{
    Ray rays[4];

    Ray& operator[](int index)
    {
        return rays[index];
    }
};

#endif