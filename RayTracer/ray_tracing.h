#pragma once

#include "trace_base.h"

class RayTracing : public TraceBase {
private:
	int maxDepth;

public:
	RayTracing(const Scene & scene_, int maxDepth_ = MAX_RAY_TRACING_DEPTH) :
		TraceBase(scene_), maxDepth(maxDepth_) {}

	glm::dvec3 color(const Ray & ray, RNG * rng = nullptr) const override;

private:
	glm::dvec3 getColor(DistRay & ray, int depth = 0) const;
	glm::dvec3 getPhongLocal(const IntersectInfo & info, const DistRay & ray, int depth = 0) const;
	glm::dvec3 getReflection(const IntersectInfo & info, const DistRay & ray, int depth = 0) const;
	glm::dvec3 getRefraction(const IntersectInfo & info, const DistRay & ray, int depth = 0) const;

	bool isShadow(Light *light, const IntersectInfo & info) const;
};