#include <algorithm>

#include "aabb.h"
#include "constants.h"
#include "utils.h"

AABB::AABB() {
	bounds[0] = max_vec3;
	bounds[1] = -max_vec3;
}

AABB::AABB(glm::dvec3 min_, glm::dvec3 max_) {
	bounds[0] = min_;
	bounds[1] = max_;
}

bool AABB::intersect(const Ray & ray, double & t) const {
	double tmax = std::numeric_limits<double>::max();
	double tmin = -tmax;
	for (int i = 0; i < 3; ++i) {
		double invD = 1.0 / ray.dir[i];
		double t0 = (bounds[0][i] - ray.ori[i]) * invD;
		double t1 = (bounds[1][i] - ray.ori[i]) * invD;
		if (invD < 0.0) std::swap(t0, t1);
		tmin = std::max(tmin, t0);
		tmax = std::min(tmax, t1);
		if (tmax <= tmin) return false;
	}
	t = tmin;
	return true;
}

bool AABB::intersect(const Ray & ray) const {
	double t;
	return intersect(ray, t);
}

void AABB::splitBox(const SplitPlane & plane, AABB & left, AABB & right) const {
	left = *this;
	left.bounds[1][plane.axis] = plane.value;
	right = *this;
	right.bounds[0][plane.axis] = plane.value;
}

double AABB::halfSA() const {
	double dist[3];
	for (int i = 0; i < 3; ++i) dist[i] = bounds[1][i] - bounds[0][i];
	return dist[0] * dist[1] + dist[1] * dist[2] + dist[2] * dist[0];
}

void AABB::expand(const AABB & box) {
	for (int i = 0; i < 3; ++i) {
		updateMin(bounds[0][i], box.bounds[0][i]);
		updateMax(bounds[1][i], box.bounds[1][i]);
	}
}

bool AABB::isPlanar(int axis) const {
	return bounds[1][axis] - bounds[0][axis] < 3e-6;
}

