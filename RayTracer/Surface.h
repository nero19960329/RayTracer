#pragma once

#include "Utils.h"
#include "Vec3.h"

class Surface {
public:
	real_t ambient, specular, shininess;
	Vec3 diffuse, emission;

public:
	Surface(real_t _ambient, const Vec3 &_diffuse, real_t _specular, real_t _shininess, const Vec3 &_emission) :
		ambient(_ambient), diffuse(_diffuse), specular(_specular), shininess(_shininess), emission(_emission) {}
	~Surface() {}

	static const Surface BLACK, WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA;
};