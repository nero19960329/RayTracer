#pragma once

#include "Vec3.h"

class Ray {
public:
	Vec3 orig, dir;

public:
	Ray(const Vec3 &_orig, const Vec3 &_dir) :
		orig(_orig), dir(_dir) {}
	~Ray() {}

	inline Vec3 getDistPoint(real_t t) const {
		return orig + dir * t;
	}
};