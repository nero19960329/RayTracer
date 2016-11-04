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

	virtual std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const = 0;

	const std::shared_ptr<Texture> &getTexture() const {
		return texture;
	}
};

struct IntersectInfo {
	Vec3 interPoint;
	Vec3 normal;
	std::shared_ptr<Surface> surface;
};

class Intersect {
protected:
	Ray ray;

	mutable real_t distToInter = std::numeric_limits<real_t>::infinity();	// distance to intersection point

public:
	explicit Intersect(const Ray &_ray) : ray(_ray) {}
	virtual ~Intersect() {}

	Intersect(const Intersect &) = delete;
	Intersect &operator = (const Intersect &) = delete;

	virtual const Object *getObj() const = 0;
	virtual real_t getDistToInter() const = 0;

	virtual bool isIntersect() const = 0;		// only finding if they're intersected
	virtual Vec3 getIntersection() const {														// find intersection point
		return ray.getDistPoint(distToInter);
	}
	virtual Vec3 getNormal() const = 0;															// get normal vector

	virtual IntersectInfo getIntersectInfo() {
		auto obj = getObj();
		assert(obj->getTexture());
		return IntersectInfo{ getIntersection(), getNormal(), getObj()->getTexture()->getSurfaceProperty() };
	}
};