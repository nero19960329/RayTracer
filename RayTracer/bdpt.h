#pragma once

#include "brdf.h"
#include "rng.h"
#include "trace_base.h"

class BidirectionalPathTracing : public TraceBase {
private:
	int minDepth;
	std::shared_ptr<BRDF> brdf;

public:
	BidirectionalPathTracing(const Scene & scene, BRDFType brdfType = LAMBERTIAN, int _minDepth = MIN_BIDIRECTIONAL_PATH_TRACING_DEPTH) :
		TraceBase(scene), minDepth(_minDepth) {
		if (brdfType == LAMBERTIAN) brdf = std::make_shared<LambertianBRDF>();
		else if (brdfType == PHONG) brdf = std::make_shared<PhongBRDF>();
	}

	glm::dvec3 color(const Ray & ray, RNG * rng) const override;

private:
	glm::dvec3 getColor(DistRay & ray, RNG * rng, int depth = 0) const;
};