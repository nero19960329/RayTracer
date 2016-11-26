#pragma once

#include "Constants.h"
#include "Material.h"
#include "Utils.h"
#include "Vec3.h"

class Surface : public Material {
public:
	Vec3 diffuse;

public:
	Surface(real_t _ambient, const Vec3 &_diffuse, real_t _specular, real_t _shininess, const Vec3 &_emission = Vec3::NONE, real_t _refl = 0.0, real_t _refr = 0.0) :
		Material(_ambient, _specular, _shininess, _refl, _refr), diffuse(_diffuse) {}
	Surface(const Material &_material, const Vec3 &_diffuse) :
		Material(_material), diffuse(_diffuse) {}
	~Surface() {}

	//static const Surface BLACK, WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA;
};