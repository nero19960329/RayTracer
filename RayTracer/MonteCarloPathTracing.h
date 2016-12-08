#pragma once

#include "BRDF.h"
#include "Light.h"
#include "RNG.h"
#include "TraceBase.h"

#include <vector>

class MonteCarloPathTracing : public TraceBase {
private:
	int minDepth;
	std::shared_ptr<BRDF> brdf;

public:
	explicit MonteCarloPathTracing(const Scene &scene, BRDFType brdfType = LAMBERTIAN, int _minDepth = MIN_MONTE_CARLO_PATH_TRACING_DEPTH) :
		TraceBase(scene), minDepth(_minDepth) {
		if (brdfType == LAMBERTIAN) brdf = std::make_shared<LambertianBRDF>();
		else if (brdfType == PHONG) brdf = std::make_shared<PhongBRDF>();
	}

	std::pair<Vec3, Vec3> color(const Ray &ray, RNGenerator *rng) const override;

private:
	std::pair<Vec3, Vec3> getColor(DistRay &ray, RNGenerator *rng, int depth = 0) const;
	std::pair<Vec3, Vec3> getReflectedRadiance(DistRay &ray, const IntersectInfo &info, RNGenerator *rng, int depth = 0) const;
	Vec3 getDirectRadiance(DistRay &ray, const IntersectInfo &info, RNGenerator *rng) const;
	Vec3 getIndirectRadiance(DistRay &ray, const IntersectInfo &info, RNGenerator *rng, int depth = 0) const;
};