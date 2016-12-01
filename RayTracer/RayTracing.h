#pragma once

#include "NeuralNetwork.h"
#include "TraceBase.h"

#include <vector>

class RayTracing : public TraceBase {
private:
	std::vector<NeuralNetwork> directNNVec, indirectNNVec;
	int maxDepth;

public:
	explicit RayTracing(const Scene &_scene, const std::vector<NeuralNetwork> &_directNNVec, const std::vector<NeuralNetwork> &_indirectNNVec, int _maxDepth = MAX_RAY_TRACING_DEPTH) : 
		TraceBase(_scene), maxDepth(_maxDepth), directNNVec(_directNNVec), indirectNNVec(_indirectNNVec) {}
	~RayTracing() {}

	std::pair<Vec3, Vec3> color(const Ray &ray, RNGenerator *rng = nullptr) const override;

private:
	Vec3 getColor(DistRay &ray, int depth = 0) const;
	Vec3 getPhongLocal(const IntersectInfo &info, const DistRay &ray, int depth = 0) const;
	Vec3 getReflection(const IntersectInfo &info, const DistRay &ray, int depth = 0) const;
	Vec3 getRefraction(const IntersectInfo &info, const DistRay &ray, int depth = 0) const;

	bool isShadow(const std::shared_ptr<Light> &light, const IntersectInfo &info) const;
};