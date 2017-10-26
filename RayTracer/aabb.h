#pragma once

#include <glm.hpp>

#include "ray.h"
#include "split_plane.h"

class AABB {
public:
	glm::dvec3 bounds[2];

public:
	AABB();
	AABB(glm::dvec3 min_, glm::dvec3 max_);

	bool intersect(const Ray &ray, double &t) const;
	bool intersect(const Ray &ray) const;
	void splitBox(const SplitPlane &plane, AABB &left, AABB &right) const;
	double halfSA() const;
	void expand(const AABB &box);
	bool isPlanar(int axis) const;
};