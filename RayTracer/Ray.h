#pragma once

#include "Constants.h"
#include "Utils.h"
#include "Vec3.h"

class Ray {
public:
	Vec3 orig, dir;
	real_t refrIdx;

public:
	Ray(const Vec3 &_orig, const Vec3 &_dir, real_t _refrIdx = VACUUM_REFRACTION_INDEX) :
		orig(_orig), dir(_dir), refrIdx(_refrIdx) {}
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