#pragma once

#include "Light.h"
#include "Object.h"

#include <list>
#include <vector>

class TraceBase;

class Scene {
public:
	std::list<Object *> objs;
	std::vector<Light *> lights;

public:
	Scene() {}

	bool isIntersect(const Ray & ray, double dist) const;
	std::shared_ptr<Intersect> getIntersect(const Ray & ray) const;

	bool isInnerPoint(const glm::dvec3 & p) const;

	AABB getAABB() const;
};