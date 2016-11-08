#pragma once

#include "Ray.h"
#include "Utils.h"
#include "Vec3.h"

class AABB {
public:
	Vec3 bounds[2];

public:
	AABB() {}
	AABB(const Vec3 &_min, const Vec3 &_max) {
		bounds[0] = _min;
		bounds[1] = _max;
	}
	~AABB() {}

	bool intersect(const Ray &ray, real_t tmin, real_t tmax) const {
		rep(i, 3) {
			real_t invD = 1.0f / ray.dir[i];
			real_t t0 = (bounds[0][i] - ray.orig[i]) * invD;
			real_t t1 = (bounds[1][i] - ray.orig[i]) * invD;
			if (invD < 0.0f) std::swap(t0, t1);
			tmin = min(t0, tmin);
			tmax = max(t1, tmax);
			if (tmax <= tmin) return false;
		}
		return true;
	}
};