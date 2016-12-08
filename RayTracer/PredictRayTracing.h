#pragma once

#include "NeuralNetwork.h"
#include "TraceBase.h"

#include <vector>

class PredictRayTracing : public TraceBase {
private:
	std::vector<NeuralNetwork> directNNVec, indirectNNVec;

public:
	explicit PredictRayTracing(const Scene &_scene, const std::vector<NeuralNetwork> &_directNNVec, const std::vector<NeuralNetwork> &_indirectNNVec) :
		TraceBase(_scene), directNNVec(_directNNVec), indirectNNVec(_indirectNNVec) {}
	~PredictRayTracing() {}

	std::pair<Vec3, Vec3> color(const Ray &ray, RNGenerator *rng = nullptr) const override;

private:
	Vec3 getColor(DistRay &ray, int depth = 0) const;
};