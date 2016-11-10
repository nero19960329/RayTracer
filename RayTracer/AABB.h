#pragma once

#include "Ray.h"
#include "Utils.h"
#include "Vec3.h"

#include <algorithm>

class AABB {
public:
	Vec3 bounds[2];

public:
	AABB() {
		bounds[0] = Vec3::max();
		bounds[1] = -Vec3::max();
	}

	AABB(const Vec3 &_min, const Vec3 &_max) {
		bounds[0] = _min;
		bounds[1] = _max;
	}

	~AABB() {}

	bool intersect(const Ray &ray, real_t &t) const {
		real_t tmax = std::numeric_limits<real_t>::max();
		real_t tmin = -tmax;
		rep(i, 3) {
			real_t invD = 1.0 / ray.dir[i];
			real_t t0 = (bounds[0][i] - ray.orig[i]) * invD;
			real_t t1 = (bounds[1][i] - ray.orig[i]) * invD;
			if (invD < 0.0) std::swap(t0, t1);
			tmin = std::max(tmin, t0);
			tmax = std::min(tmax, t1);
			if (tmax <= tmin) return false;
		}
		t = tmin;
		return true;
	}

	bool intersect(const Ray &ray) const {
		real_t t;
		return intersect(ray, t);
	}

	void expand(const AABB &box) {
		rep(i, 3) {
			updateMin(bounds[0][i], box.bounds[0][i]);
			updateMax(bounds[1][i], box.bounds[1][i]);
		}
	}

	int getLongestAxis() const {
		real_t dist[3];
		rep(i, 3) dist[i] = bounds[1][i] - bounds[0][i];
		if (dist[0] >= dist[1] && dist[0] >= dist[2]) return 0;
		else if (dist[1] >= dist[2]) return 1;
		else return 2;
	}

	Vec3 getMid() const {
		return (bounds[0] + bounds[1]) * 0.5;
	}
};