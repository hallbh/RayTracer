#ifndef _RAY_GENERATOR_H
#define _RAY_GENERATOR_H

#include "Camera.h"
#include "Ray.h"

#include <cmath>

class RayGenerator
{
private:
    Camera *camera;
    float invImageWidth;
    float invImageHeight;
    float fromCameraToLeft;
    float fromCameraToRight;
    float fromCameraToTop;
    float fromCameraToBottom;
    float d;

public:
    RayGenerator(Camera *camera, int xDim, int yDim);

    Ray getRay(int x, int y);
    void getRayBundle(int x, int y, rayBundle &bundle);

};

RayGenerator::RayGenerator(Camera *camera, int xDim, int yDim)
    : camera(camera), invImageWidth(1.0f/xDim), invImageHeight(1.0f/yDim)
{
    this->fromCameraToLeft = -xDim / 2.0f;
    this->fromCameraToRight = xDim / 2.0f;
    this->fromCameraToTop = yDim / 2.0f;
    this->fromCameraToBottom = -yDim / 2.0f;
    this->d = xDim / 2.0f / tanf(this->camera->getFOV()/2);
}

Ray RayGenerator::getRay(int x, int y)
{
    float u = this->fromCameraToLeft + (this->fromCameraToRight - this->fromCameraToLeft) * (x + 0.5f) * this->invImageWidth;
    float v = this->fromCameraToBottom + (this->fromCameraToTop - this->fromCameraToBottom) * (y + 0.5f) * this->invImageHeight;
    //float d = this->imageWidth / 2.0f / tanf(this->camera.getFOV()/2);

    Vec3 direction = u * this->camera->getu() + v * this->camera->getv() - d * this->camera->getw();

    // s = direction + e
    // direction = s - e

    return Ray(this->camera->getPosition(), direction);
}

void RayGenerator::getRayBundle(int x, int y, rayBundle &bundle)
{
    float uStuff = (this->fromCameraToRight - this->fromCameraToLeft) * this->invImageWidth;
    float vStuff = (this->fromCameraToTop - this->fromCameraToBottom) * this->invImageHeight;

    float xOff[] = {0,1,0,1};
    float yOff[] = {0,0,1,1};
    float u[4], v[4];

    const Vec3 camU = this->camera->getu(), 
    camV = this->camera->getv(), 
    camW = this->camera->getw(), 
    camPos = this->camera->getPosition();
    
    for (int i = 0; i < 4; i++)
    {
        u[i] = this->fromCameraToLeft + uStuff * (x + 0.5f + xOff[i]);
        v[i] = this->fromCameraToBottom + vStuff * (y + 0.5f + yOff[i]);
        //float d = this->imageWidth / 2.0f / tanf(this->camera.getFOV()/2);

        for (int j = 0; j < 3; j++)
        {
            bundle.rays[i].direction[j] = u[i] * camU[j] + v[i] * camV[j] - d * camW[j];
            bundle.rays[i].origin[j] = camPos[j];

            // bundle.rays[i].direction[1] = u[i] * camU[1] + v[i] * camV[1] - d * camW[1];
            // bundle.rays[i].origin[1] = camPos[1];

            // bundle.rays[i].direction[2] = u[i] * camU[2] + v[i] * camV[2] - d * camW[2];
            // bundle.rays[i].origin[2] = camPos[2];
        }
    }

    // s = direction + e
    // direction = s - e

    //return bundle;
}

#endif
