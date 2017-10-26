#pragma once

#include "brdf.h"
#include "rng.h"
#include "trace_base.h"

#include <vector>

class MonteCarloPathTracing : public TraceBase {
private:
	int minDepth;
	std::shared_ptr<BRDF> brdf;

public:
	MonteCarloPathTracing(const Scene & scene, BRDFType brdfType = LAMBERTIAN, int _minDepth = MAX_MONTE_CARLO_PATH_TRACING_DEPTH) :
		TraceBase(scene), minDepth(_minDepth) {
		if (brdfType == LAMBERTIAN) brdf = std::make_shared<LambertianBRDF>();
		else if (brdfType == PHONG) brdf = std::make_shared<PhongBRDF>();
	}

	glm::dvec3 color(const Ray & ray, RNG * rng) const override;

private:
	glm::dvec3 getColor(DistRay & ray, RNG * rng, int depth = 0) const;
	glm::dvec3 getRayRadiance(DistRay & ray, std::shared_ptr<Intersect> intersect, RNG * rng, int depth = 0) const;
	glm::dvec3 getDirectRadiance(DistRay & ray, std::shared_ptr<Intersect> intersect, RNG * rng) const;
	glm::dvec3 getIndirectRadiance(DistRay & ray, std::shared_ptr<Intersect> intersect, RNG * rng, int depth = 0) const;
};