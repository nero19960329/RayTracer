#pragma once

#include "Object.h"
#include "Vec3.h"

class SphereIntersect;

class Sphere : public Object {
	friend SphereIntersect;

protected:
	Vec3 center;
	real_t radius;

public:
	Sphere(const std::shared_ptr<Texture> &_texture, const Vec3 &_center, real_t _radius) :
		Object(_texture), center(_center), radius(_radius) {
		assert(radius > 0);
	}
	~Sphere() {}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;
};

class SphereIntersect : public Intersect {
private:
	Sphere sphere;

	mutable real_t distToProjCenter;
	mutable real_t halfCord2;
	mutable Vec3 interPoint;
	mutable Vec3 normal;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const override;

	const Object *getObj() const override {
		return &sphere;
	}

public:
	SphereIntersect(const Sphere &_sphere, const Ray &_ray) : Intersect(_ray), sphere(_sphere) {}
	~SphereIntersect() {}

	real_t getDistToInter() const override;

	bool isIntersect() const override;
	Vec3 getNormal() const override;
};