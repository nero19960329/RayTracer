#pragma once

#include "Light.h"
#include "Object.h"

#include <list>
#include <tuple>
#include <vector>

class TraceBase;

class Scene {
public:
	std::list<Object *> objs;
	std::vector<Light *> lights;

private:
	std::vector<double> lightCDF;

public:
	Scene() {}

	bool isIntersect(const Ray & ray, double dist) const;
	std::shared_ptr<Intersect> getIntersect(const Ray & ray) const;

	bool isInnerPoint(const glm::dvec3 & p) const;

	AABB getAABB() const;

	void computeLightCDF();
	std::tuple<int, double> lightIdxSample(RNG & rng) const;
};