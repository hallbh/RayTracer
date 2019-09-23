#ifndef _SPHERE_H
#define _SPHERE_H

#include "BoundingBox.h"
#include "Surface.h"

#include "rayHit.h"

#include "libs/Matrix.h"

#include <math.h>
#include <string>

#define VOXEL_SIZE -0.1f

class Sphere : public Surface
{
private:
    Vec3 center;
    Vec3 equatorNormal;
    Vec3 upNormal;
    float radius;

public:
    Sphere(Vec3 center, Vec3 equatorNormal, Vec3 upNormal, float radius, std::string materialID);

    virtual bool hit(Ray ray, float startTime, rayHit &record);

    virtual Vec3 getCentroid();
    virtual BoundingBox getBoundingBox();
};

Sphere::Sphere(Vec3 center, Vec3 equatorNormal, Vec3 upNormal, float radius, std::string materialID)
    : Surface(materialID), center(center), radius(radius)
{
    this->equatorNormal = Mat::normalize(equatorNormal);
    this-> upNormal = Mat::normalize(upNormal);
}

bool Sphere::hit(Ray ray, float startTime, rayHit &record)
{
    Vec3 d = ray.getDirection();
    Vec3 e = ray.positionAtTime(0);
    float dDotemc = Mat::dot(d, e - this->center);
    float dDotd = Mat::dot(d, d);
    float discriminant = dDotemc * dDotemc - dDotd * (Mat::dot(e - this->center, e - this->center) - this->radius * this->radius);

    if (discriminant < 0)
        return false;

    discriminant = sqrtf(discriminant);
    float time;

    if ((-dDotemc - discriminant) > 0)
    {
        time = (-dDotemc - discriminant) / dDotd;
        //Hit the near side of the sphere
    }
    else
    {
        time = (-dDotemc + discriminant) / dDotd;
        //Hit the far side of the sphere
    }

    if (!(startTime < time && time < record.intersectionTime))
        return false;

    // if (VOXEL_SIZE <= 0)
    {
        record.intersectionTime = time;
        record.intersectionPoint = ray.positionAtTime(time);
        record.surfaceNormal = Mat::normalize(record.intersectionPoint - this->center);
        record.materialID = this->materialName;
        return true;
    }

    //Voxel rendering stuff

    // bool hit = false;

    // Vec3 centerVoxelPoint = ray.positionAtTime(time) / VOXEL_SIZE;

    // for (int j = 0; j < 3; j++)
    // {
    //     centerVoxelPoint[j] = floorf(centerVoxelPoint[j]) * VOXEL_SIZE;
    // }

    // int xDir, yDir, zDir;
    // xDir = ray.getDirection()[0] < 0 ? -1 : 1;
    // yDir = ray.getDirection()[1] < 0 ? -1 : 1;
    // zDir = ray.getDirection()[2] < 0 ? -1 : 1;

    // int failsafe = 0;
    // float lastTime = 0;

    // rayHit voxelInfo;

    // while (time < (-dDotemc + discriminant) / dDotd && time != lastTime && failsafe < 128)
    // {
    //     bool inside;
    //     float exitTime;

    //     for (int i = 0; i < 3; i++)
    //     {
    //         inside = true;

    //         Vec3 voxelPoint = centerVoxelPoint;
    //         voxelPoint[0] += VOXEL_SIZE * (i == 0) * xDir;
    //         voxelPoint[1] += VOXEL_SIZE * (i == 1) * yDir;
    //         voxelPoint[2] += VOXEL_SIZE * (i == 2) * zDir;

    //         for (int l = 0; l < 8; l++)
    //         {
    //             Vec3 corner = voxelPoint;
    //             corner[0] += VOXEL_SIZE * (0b001 & l);
    //             corner[1] += VOXEL_SIZE * (0b010 & l);
    //             corner[2] += VOXEL_SIZE * (0b100 & l);

    //             if (Mat::dot(corner - this->center, corner - this->center) > radius * radius)
    //             {
    //                 inside = false;
    //                 break;
    //             }
    //         }

    //         BoundingBox bb(voxelPoint, voxelPoint + VOXEL_SIZE);
            
    //         if (bb.hit(ray, startTime, endTime, voxelInfo, &exitTime))
    //         {
    //             hit = true;
    //             centerVoxelPoint = voxelPoint;
    //             //endTime = record.intersectionTime;
    //         }

    //         if (inside && hit)
    //             break;

            
    //         hit = false;
    //     }

    //     if (inside && hit)
    //         break;

    //     lastTime = time;
    //     time = exitTime;

    //     failsafe++;
    // }

    // if (!hit)
    //     return false;

    // record = voxelInfo;
    
    // record.materialID = this->materialName;
    
    // return true;
}

Vec3 Sphere::getCentroid()
{
    return this->center;
}

BoundingBox Sphere::getBoundingBox()
{
    return BoundingBox(this->center - this->radius, this->center + this->radius);
}

#endif 
