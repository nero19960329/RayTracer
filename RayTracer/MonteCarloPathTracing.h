#pragma once

#include "TraceBase.h"

class MonteCarloPathTracing : public TraceBase {
private:
	int minDepth;

public:
	explicit MonteCarloPathTracing(const Scene &scene, int _minDepth = MIN_MONTE_CARLO_PATH_TRACING_DEPTH) :
		TraceBase(scene), minDepth(_minDepth) {}

	Vec3 color(const Ray &ray) const override;

private:
	Vec3 getColor(DistRay &ray, int depth = 0) const;
	Vec3 getReflectedRadiance(DistRay &ray, const IntersectInfo &info, int depth = 0) const;
	Vec3 getDirectRadiance(DistRay &ray) const;
	Vec3 getIndirectRadiance(DistRay &ray) const;
};