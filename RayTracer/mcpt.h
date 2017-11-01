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
	glm::dvec3 getColor(Ray & ray, RNG * rng, int depth = 0) const;
	glm::dvec3 getRayRadiance(Ray & ray, std::shared_ptr<Intersect> intersect, RNG * rng) const;
	glm::dvec3 getReflectedRadiance(Ray & ray, std::shared_ptr<Intersect> intersect, RNG * rng, int depth = 0) const;
};