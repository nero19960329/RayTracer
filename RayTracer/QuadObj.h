#pragma once

#include "Object.h"
#include "Quadrilateral.h"

class QuadIntersect;

class QuadObj : public Object {
	friend class QuadIntersect;

protected:
	Quadrilateral quad;

public:
	QuadObj() : Object(nullptr) {}
	QuadObj(const std::shared_ptr<Texture> &_texture, const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &d) :
		Object(_texture), quad(a, b, c, d) {}

	AABB getAABB() const override {
		if (aabb.bounds[1] != -Vec3::max()) return aabb;
		Vec3 minVec, maxVec;
		rep(k, 3) {
			minVec[k] = std::min(std::min(std::min(quad.a[k], quad.b[k]), quad.c[k]), quad.d[k]);
			maxVec[k] = std::max(std::max(std::max(quad.a[k], quad.b[k]), quad.c[k]), quad.d[k]);
		}
		aabb = AABB{ minVec - Vec3::epsilons(), maxVec + Vec3::epsilons() };
		return aabb;
	}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;

	bool liesInPlane(const SplitPlane &p) const override {
		return (fabs(quad.a[p.axis] - p.value) < epsilon &&
			fabs(quad.b[p.axis] - p.value) < epsilon &&
			fabs(quad.c[p.axis] - p.value) < epsilon &&
			fabs(quad.d[p.axis] - p.value) < epsilon);
	}
};

class QuadIntersect : public Intersect {
private:
	const QuadObj &quadObj;

	mutable Vec3 normal;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const override { return nullptr; }

	const Object *getObj() const override { return &quadObj; }

public:
	QuadIntersect(const QuadObj &_quadObj, const Ray &_ray) :
		Intersect(_ray), quadObj(_quadObj) {}

	bool isIntersect() const override;
	real_t getDistToInter() const override;
	Vec3 getNormal() const override;
};