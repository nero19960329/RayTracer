#pragma once

#include <glm.hpp>

#include "constants.h"

class Ray {
public:
	glm::dvec3 ori, dir;
	double refrIdx;

public:
	Ray() {}
	Ray(glm::dvec3 ori_, glm::dvec3 dir_, double refrIdx_ = VACUUM_REFRACTION_INDEX) :
		ori(ori_), dir(dir_), refrIdx(refrIdx_) {}

	glm::dvec3 getDistPoint(double t) const { return ori + dir * t; }
};

class DistRay : public Ray {
public:
	double dist;

public:
	DistRay(const Ray &ray_, double dist_ = 0.0) :
		Ray(ray_), dist(dist_) {}
};