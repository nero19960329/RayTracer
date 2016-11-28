#pragma once

#include "TraceBase.h"

class RayTracing : public TraceBase {
private:
	int maxDepth;

public:
	explicit RayTracing(const Scene &_scene, int _maxDepth = MAX_RAY_TRACING_DEPTH) : TraceBase(_scene), maxDepth(_maxDepth) {}
	~RayTracing() {}

	std::pair<Vec3, Vec3> color(const Ray &ray) const override;

private:
	Vec3 getColor(DistRay &ray, int depth = 0) const;
	Vec3 getPhongLocal(const IntersectInfo &info, const DistRay &ray, int depth = 0) const;
	Vec3 getReflection(const IntersectInfo &info, const DistRay &ray, int depth = 0) const;
	Vec3 getRefraction(const IntersectInfo &info, const DistRay &ray, int depth = 0) const;

	bool isShadow(const std::shared_ptr<Light> &light, const IntersectInfo &info) const;
};