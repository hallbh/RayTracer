#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "libs/Matrix.h"

#include <string>

class Material
{
public:
    const std::string name;
	const std::string texture_filename;
	const Vec3 amb;
	const Vec3 diff;
	const Vec3 spec;
	const double reflect;
	const double refract;
	const double trans;
	const double shiny;
	const double glossy;
	const double refract_index;

    Material(std::string name, std::string texture_filename, Vec3 amb, Vec3 diff, Vec3 spec, double reflect, 
        double refract, double trans, double shiny, double glossy, double refract_index)
        : name(name), texture_filename(texture_filename), amb(amb), diff(diff), spec(spec), reflect(reflect), refract(refract),
            trans(trans), shiny(shiny), glossy(glossy), refract_index(refract_index)
    {}

    Material(const Material& mat)
        : name(mat.name), texture_filename(mat.texture_filename), amb(mat.amb), diff(mat.diff), spec(mat.spec), reflect(mat.reflect), refract(mat.refract),
            trans(mat.trans), shiny(mat.shiny), glossy(mat.glossy), refract_index(mat.refract_index)
    {}
};

#endif
