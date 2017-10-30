#pragma once

#include "rng.h"
#include "trace_base.h"

#include <vector>

class MonteCarloPathTracing : public TraceBase {
private:
	int minDepth;

public:
	MonteCarloPathTracing(const Scene & scene, int _minDepth = MIN_MONTE_CARLO_PATH_TRACING_DEPTH) :
		TraceBase(scene), minDepth(_minDepth) {}

	glm::dvec3 color(const Ray & ray, RNG * rng) const override;

private:
	glm::dvec3 getColor(DistRay & ray, RNG * rng, int depth = 0) const;
	glm::dvec3 getRayRadiance(DistRay & ray, std::shared_ptr<Intersect> intersect, RNG * rng) const;
	glm::dvec3 getReflectedRadiance(DistRay & ray, std::shared_ptr<Intersect> intersect, RNG * rng, int depth = 0) const;
};