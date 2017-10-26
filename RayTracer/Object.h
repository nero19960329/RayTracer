#pragma once

#include <glm.hpp>

#include <memory>

#include "aabb.h"
#include "constants.h"
#include "material.h"
#include "ray.h"
#include "split_plane.h"
#include "texture.h"
#include "utils.h"

class Intersect;

class Object {
	friend class Intersect;

protected:
	Texture * texture;

	mutable AABB aabb;

public:
	Object(Texture * texture_) : texture(texture_) {}
	virtual ~Object() {}

	virtual bool isInfinity() const { return false; }
	virtual bool hasInside() const { return false; }

	virtual std::shared_ptr<Intersect> getTrace(const Ray &ray, double dist = std::numeric_limits<double>::max()) const = 0;

	virtual void setTexture(Texture * texture_) { texture = texture_; }
	const Texture *getTexture() const { return texture; }

	virtual AABB getAABB() const { return{}; }

	virtual bool liesInPlane(const SplitPlane &p) const { return false; }
	virtual AABB clipToBox(const AABB &aabb) const {
		AABB B = getAABB();
		for (int i = 0; i < 3; ++i) {
			updateMax(B.bounds[0][i], aabb.bounds[0][i]);
			updateMin(B.bounds[1][i], aabb.bounds[1][i]);
		}
		return B;
	}
};

struct IntersectInfo {
	glm::dvec3 interPoint;
	glm::dvec3 normal;
	std::shared_ptr<Surface> material;
	double nextRefrIdx;
};

class Intersect {
protected:
	const Ray &ray;

	mutable double distToInter = std::numeric_limits<double>::infinity();
	mutable glm::dvec3 interPoint = inf_vec3;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProp() const = 0;

public:
	explicit Intersect(const Ray &ray_) : ray(ray_) {}
	virtual ~Intersect() {}

	Intersect(const Intersect &) = delete;
	Intersect &operator = (const Intersect &) = delete;
	
	virtual const Object * getObj() const = 0;
	virtual bool hasInside() const { return getObj()->hasInside(); }
	virtual double getDistToInter() const = 0;
	virtual bool isIntersect() const = 0;
	virtual glm::dvec3 getIntersection() const { return (interPoint = ray.getDistPoint(distToInter)); }
	virtual glm::dvec3 getNormal() const = 0;
	virtual double getNextRefrIdx() const { return ray.refrIdx; }

	virtual IntersectInfo getIntersectInfo() {
		auto obj = getObj();
		return IntersectInfo{ getIntersection(), getNormal(), getSurface(), getNextRefrIdx() };
	}

	virtual std::shared_ptr<Surface> getSurface() const {
		auto texture = getObj()->getTexture();
		if (!texture) return nullptr;
		auto surface = texture->getProp();
		if (surface) return surface;
		return getInterPointSurfaceProp();
	}
};