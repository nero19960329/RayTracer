#pragma once

#include "Ray.h"
#include "Texture.h"

class Intersect;

class Object {
	friend class Intersect;

private:
	std::shared_ptr<Texture> texture;

public:
	explicit Object(const std::shared_ptr<Texture> &_texture) : texture(_texture) {}
	virtual ~Object() {}

	virtual bool isInfinity() const {
		return false;
	}

	virtual bool hasInside() const {
		return false;
	}

	virtual std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const = 0;

	const std::shared_ptr<Texture> &getTexture() const {
		return texture;
	}

	void setTexture(const std::shared_ptr<Texture> &_texture) {
		texture = _texture;
	}
};

struct IntersectInfo {
	Vec3 interPoint;
	Vec3 normal;
	std::shared_ptr<Surface> surface;
	real_t nextRefrIdx;
};

class Intersect {
protected:
	const Ray &ray;

	mutable real_t distToInter = std::numeric_limits<real_t>::infinity();	// distance to intersection point
	mutable Vec3 interPoint = Vec3::infinity();

	virtual const Object *getObj() const = 0;
	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const = 0;

public:
	explicit Intersect(const Ray &_ray) : ray(_ray) {}
	virtual ~Intersect() {}

	Intersect(const Intersect &) = delete;
	Intersect &operator = (const Intersect &) = delete;

	virtual real_t getDistToInter() const = 0;

	virtual bool isIntersect() const = 0;		// only finding if they're intersected
	virtual Vec3 getIntersection() const {		// find intersection point
		interPoint = ray.getDistPoint(distToInter);
		return interPoint;
	}

	virtual IntersectInfo getIntersectInfo() {
		auto obj = getObj();
		assert(obj->getTexture());
		return IntersectInfo{ getIntersection(), getNormal(), getSurface(), getNextRefractionIndex() };
	}

private:
	virtual Vec3 getNormal() const = 0;			// get normal vector

	std::shared_ptr<Surface> getSurface() const {
		auto surface = getObj()->getTexture()->getSurfaceProperty();
		if (surface) return surface;
		return getInterPointSurfaceProperty();
	};

	virtual real_t getNextRefractionIndex() const {
		return ray.refrIdx;
	}
};