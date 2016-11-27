#pragma once

#include "BRDF.h"
#include "TraceBase.h"

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

	Vec3 color(const Ray &ray) const override;

private:
	Vec3 getColor(DistRay &ray, int depth = 0) const;
	Vec3 getReflectedRadiance(DistRay &ray, const IntersectInfo &info, int depth = 0) const;
	Vec3 getDirectRadiance(DistRay &ray) const;
	Vec3 getIndirectRadiance(DistRay &ray) const;
};