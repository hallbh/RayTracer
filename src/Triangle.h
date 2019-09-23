#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "BoundingBox.h"
#include "Surface.h"

#include "libs/Matrix.h"

#include <algorithm>
#include <string>

class Triangle : public Surface
{
private:
    Vec3 a;
    Vec3 b;
    Vec3 c;
    Vec3 centroid;
    Vec3 normal;

public:
    Triangle(Vec3 a, Vec3 b, Vec3 c, std::string materialID);

    virtual bool hit(Ray ray, float startTime, rayHit &record);

    virtual Vec3 getCentroid();
    virtual BoundingBox getBoundingBox();
};

Triangle::Triangle(Vec3 a, Vec3 b, Vec3 c, std::string materialID)
    : Surface(materialID), a(a), b(b), c(c)
{
    this->normal = Mat::normalize(Mat::cross(b - a, c - b));
    this->centroid = (a + b + c) / 3;
}

bool Triangle::hit(Ray ray, float startTime, rayHit &record)
{
    //check the denominator first to avoid division by 0
    if (Mat::dot(ray.getDirection(), this->normal) == 0)
        return false;

    float t = Mat::dot(this->a - ray.positionAtTime(0), this->normal) / Mat::dot(ray.getDirection(), this->normal);

    if (t < startTime)
        return false;

    Vec3 x = ray.positionAtTime(t);

    if (Mat::dot(Mat::cross(this->b - this->a, x - this->a), this->normal) < 0)
        return false;

    if (Mat::dot(Mat::cross(this->c - this->b, x - this->b), this->normal) < 0)
        return false;

    if (Mat::dot(Mat::cross(this->a - this->c, x - this->c), this->normal) < 0)
        return false;

    if (!(startTime < t && t < record.intersectionTime))
        return false;

    record.intersectionTime = t;
    record.intersectionPoint = x;
    record.surfaceNormal = this->normal;
    record.materialID = this->materialName;

    return true;

}

Vec3 Triangle::getCentroid()
{
    return this->centroid;
}

BoundingBox Triangle::getBoundingBox()
{
    Vec3 min;
    min[0] = std::min({a[0], b[0], c[0]});
    min[1] = std::min({a[1], b[1], c[1]});
    min[2] = std::min({a[2], b[2], c[2]});

    Vec3 max;
    max[0] = std::max({a[0], b[0], c[0]});
    max[1] = std::max({a[1], b[1], c[1]});
    max[2] = std::max({a[2], b[2], c[2]});

    return BoundingBox(min, max);
}

#endif
