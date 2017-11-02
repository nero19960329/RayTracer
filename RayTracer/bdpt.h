#pragma once

#include "bsdf.h"
#include "rng.h"
#include "trace_base.h"

#include <vector>

class PathNode {
public:
	const Object * obj;
	IntersectInfo info;
	glm::dvec3 f;
	double p;

public:
	PathNode() {}
	PathNode(const Object * obj_, IntersectInfo info_, glm::dvec3 f_, double p_) : 
		obj(obj_), info(info_), f(f_), p(p_) {}
};

class BidirectionalPathTracing : public TraceBase {
private:
	int minDepth;

public:
	BidirectionalPathTracing(const Scene & scene, int _minDepth = MIN_BIDIRECTIONAL_PATH_TRACING_DEPTH) :
		TraceBase(scene), minDepth(_minDepth) {}

	glm::dvec3 color(const Ray & ray, RNG * rng) const override;

private:
	glm::dvec3 getColor(Ray & ray, RNG * rng) const;

	std::vector<PathNode> getEyePathNodes(Ray & ray, RNG * rng) const;
	std::vector<PathNode> getLightPathNodes(RNG * rng) const;
	std::vector<PathNode> getPathNodes(const std::vector<PathNode> & nodes, Ray & startRay, RNG * rng) const;

	std::tuple<glm::dvec3, double> evalPath(RNG * rng, const std::vector<PathNode> & eyeNodes, int eyeIdx, const std::vector<PathNode> & lightNodes, int lightIdx) const;
};