#pragma once

#include "Light.h"
#include "Object.h"

#include <list>
#include <vector>

enum TraceType { RT, MCPT };

class TraceBase;

class Scene {
public:
	std::list<std::shared_ptr<Object>> objs;
	std::vector<std::shared_ptr<Light>> lights;

public:
	Scene() {}
	virtual ~Scene() {}

	std::shared_ptr<TraceBase> getTracer(TraceType traceType, BRDFType brdfType) const;

	void addLight(const std::shared_ptr<Light> &light);
	void addObject(const std::shared_ptr<Object> &object);

	bool isIntersect(const Ray &ray, real_t dist) const;
	std::shared_ptr<Intersect> getIntersect(const Ray &ray) const;

	bool isInnerPoint(const Vec3 &p) const;
};