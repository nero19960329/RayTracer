#pragma once

#include "Vec3.h"

class Ray {
public:
	Vec3 orig, dir;

public:
	Ray(const Vec3 &_orig, const Vec3 &_dir) :
		orig(_orig), dir(_dir) {}
	virtual ~Ray() {}

	inline Vec3 getDistPoint(real_t t) const {
		return orig + dir * t;
	}
};

class DistRay : public Ray {
public:
	real_t dist;

public:
	DistRay(const Ray &_ray, real_t _dist = 0.0) :
		Ray(_ray), dist(_dist) {}
	~DistRay() {}
};