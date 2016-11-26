#pragma once

#include "Constants.h"
#include "Utils.h"
#include "Vec3.h"

class Material {
public:
	real_t ambient, specular, shininess;
	real_t refl, refr;
	Vec3 emission;

public:
	explicit Material(real_t _ambient = DEFAULT_AMBIENT, real_t _specular = DEFAULT_SPECULAR, real_t _shininess = DEFAULT_SHININESS, real_t _refl = 0.0, real_t _refr = 0.0, const Vec3 &_emission = Vec3::NONE) :
		ambient(_ambient), specular(_specular), shininess(_shininess), refl(_refl), refr(_refr), emission(_emission) {}
	virtual ~Material() {}
	
	static const Material FLOOR, A_BIT_MIRROR, MIRROR, MCPT_FROSTED_MIRROR, TRANSPARENT_MATERIAL, PLASTIC;
};