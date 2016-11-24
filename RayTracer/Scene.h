#pragma once

#include "Light.h"
#include "Object.h"

#include <list>
#include <vector>

enum TraceType { RAY_TRACING };

class TraceBase;

class Scene {
public:
	std::list<std::shared_ptr<Object>> objs;
	std::vector<std::shared_ptr<Light>> lights;

public:
	Scene() {}
	virtual ~Scene() {}

	std::shared_ptr<TraceBase> getTracingType(TraceType type) const;

	void addLight(const std::shared_ptr<Light> &light);
	void addObject(const std::shared_ptr<Object> &object);

	bool isIntersect(const Ray &ray, real_t dist) const;
	std::shared_ptr<Intersect> getIntersect(const Ray &ray) const;
};

class TraceBase {
protected:
	const Scene &scene;

public:
	TraceBase(const Scene &_scene) : scene(_scene) {}
	virtual ~TraceBase() {}

	virtual Vec3 color(const Ray &ray) const = 0;
};

class RayTracing : public TraceBase {
private:
	int maxDepth;

public:
	explicit RayTracing(const Scene &_scene, int _maxDepth = MAX_TRACING_DEPTH) : TraceBase(_scene), maxDepth(_maxDepth) {}
	~RayTracing() {}

	Vec3 color(const Ray &ray) const override;

private:
	Vec3 getColor(DistRay &ray, int depth = 0) const;
	Vec3 getPhongLocal(const IntersectInfo &info, const DistRay &ray, int depth = 0) const;
	Vec3 getReflection(const IntersectInfo &info, const DistRay &ray, int depth = 0) const;
	Vec3 getRefraction(const IntersectInfo &info, const DistRay &ray, int depth = 0) const;

	bool isShadow(const std::shared_ptr<Light> &light, const IntersectInfo &info) const;
};