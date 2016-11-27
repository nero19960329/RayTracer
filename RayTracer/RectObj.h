#pragma once

#include "Plane.h"
#include "RectGeo.h"
#include "Face.h"

#include <array>

class RectIntersect;

class RectObj : public Plane {
	friend class RectIntersect;

protected:
	RectGeo rectangle;

public:
	RectObj() : Plane() {}
	RectObj(const std::shared_ptr<Texture> &_texture, const Vec3 &_center, const Vec3 &_x, const Vec3 &_y, real_t _radius) :
		Plane(_texture, _center, _x, _y), rectangle(_center, _x, _y, _radius) {}

	AABB getAABB() const override {
		if (aabb.bounds[1] != -Vec3::max()) return aabb;
		Vec3 minVec, maxVec;
		rep(k, 3) {
			minVec[k] = std::min(std::min(std::min(rectangle.a[k], rectangle.b[k]), rectangle.c[k]), rectangle.d[k]);
			maxVec[k] = std::max(std::max(std::max(rectangle.a[k], rectangle.b[k]), rectangle.c[k]), rectangle.d[k]);
		}
		aabb = AABB{ minVec - Vec3::epsilons(), maxVec + Vec3::epsilons() };
		return aabb;
	}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;

	bool liesInPlane(const SplitPlane &p) const override {
		return (fabs(rectangle.a[p.axis] - p.value) < epsilon &&
			fabs(rectangle.b[p.axis] - p.value) < epsilon &&
			fabs(rectangle.c[p.axis] - p.value) < epsilon &&
			fabs(rectangle.d[p.axis] - p.value) < epsilon);
	}
};

class RectIntersect : public PlaneIntersect {
private:
	const RectObj &rectObj;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const override { return nullptr; }

	const Object *getObj() const override { return &rectObj; }

public:
	RectIntersect(const RectObj &_rectObj, const Ray &_ray) :
		PlaneIntersect(_rectObj, _ray), rectObj(_rectObj) {}

	bool isIntersect() const override;
	real_t getDistToInter() const override;
	Vec3 getNormal() const override;
};