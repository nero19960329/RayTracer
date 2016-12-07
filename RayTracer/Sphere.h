#pragma once

#include "Constants.h"
#include "Object.h"
#include "PureSphere.h"
#include "Vec3.h"

class SphereIntersect;

class Sphere : public Object {
	friend class SphereIntersect;

protected:
	PureSphere pureSphere;
	real_t innerRefrIdx;

public:
	Sphere(const std::shared_ptr<Texture> &_texture, int _num, const Vec3 &_center, real_t _radius, real_t _innerRefrIdx = VACUUM_REFRACTION_INDEX) :
		Object(_texture, _num), pureSphere(_center, _radius), innerRefrIdx(_innerRefrIdx) {}
	Sphere(const std::shared_ptr<Texture> &_texture, int _num, const PureSphere &_pureSphere, real_t _innerRefrIdx = VACUUM_REFRACTION_INDEX) :
		Object(_texture, _num), pureSphere(_pureSphere), innerRefrIdx(_innerRefrIdx) {}
	~Sphere() {}

	bool hasInside() const override { return true; }

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;

	AABB getAABB() const override {
		if (aabb.bounds[1] != -Vec3::max()) return aabb;
		Vec3 radiusVec{ pureSphere.radius, pureSphere.radius, pureSphere.radius };
		aabb = AABB{ pureSphere.center - radiusVec, pureSphere.center + radiusVec };
		return aabb;
	}
};

class SphereIntersect : public Intersect {
private:
	const Sphere &sphere;

	mutable bool toward, inside;
	mutable real_t distToProjCenter;
	mutable real_t halfCord2;
	mutable Vec3 interPoint;
	mutable Vec3 normal;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const override;

	const Object *getObj() const override { return &sphere; }

public:
	SphereIntersect(const Sphere &_sphere, const Ray &_ray) : Intersect(_ray), sphere(_sphere) {}
	~SphereIntersect() {}

	real_t getDistToInter() const override;
	bool isIntersect() const override;

private:
	Vec3 getNormal() const override;
	real_t getNextRefractionIndex() const override;
};