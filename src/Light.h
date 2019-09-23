#ifndef _LIGHT_H
#define _LIGHT_H

#include "libs/Matrix.h"

#include <string>

class Light
{
private:
    Vec3 position;
    std::string materialID;

public:
    Light(Vec3 position, std::string material);

    Vec3 getPosition();
    std::string getMaterialName();
};

Light::Light(Vec3 position, std::string material)
    : position(position), materialID(material)
{}

Vec3 Light::getPosition()
{
    return this->position;
}

std::string Light::getMaterialName()
{
    return this->materialID;
}

#endif
