#pragma once

#include "brdf.h"
#include "rng.h"
#include "trace_base.h"

#include <vector>

class PathNode {
public:
	enum NodeProp { Eye, Light };
	const Object * obj;
	IntersectInfo info;
	NodeProp eyeOrLight;
	int idx;
	glm::dvec3 f;
	double p;

public:
	PathNode() {}
	PathNode(const Object * obj_, IntersectInfo info_, NodeProp eyeOrLight_, int idx_, glm::dvec3 f_, double p_) : 
		obj(obj_), info(info_), eyeOrLight(eyeOrLight_), idx(idx_), f(f_), p(p_) {}
};

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
	glm::dvec3 getColor(DistRay & ray, RNG * rng) const;

	std::vector<PathNode> getEyePathNodes(DistRay & ray, RNG * rng) const;
	std::vector<PathNode> getLightPathNodes(RNG * rng) const;
	std::vector<PathNode> getPathNodes(const std::vector<PathNode> & nodes, DistRay & startRay, RNG * rng) const;

	std::tuple<glm::dvec3, double> evalPath(RNG * rng, const std::vector<PathNode> & eyeNodes, int eyeIdx, const std::vector<PathNode> & lightNodes, int lightIdx) const;
};