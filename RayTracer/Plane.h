#pragma once

#include "InfPlane.h"
#include "Object.h"
#include "Vec3.h"

class PlaneIntersect;

class Plane : public Object {
	friend class PlaneIntersect;

protected:
	InfPlane infPlane;

public:
	Plane() : Object(nullptr) {}

	Plane(const std::shared_ptr<Texture> &_texture, const Vec3 &_normal, real_t _offset, bool isNormalized = true) :
		Object(_texture), infPlane(_normal, _offset) {
		if (!isNormalized) infPlane.normal.normalize();
	}

	Plane(const std::shared_ptr<Texture> &_texture, const Vec3 &_normal, const Vec3 &_point, bool isNormalized = true) :
		Object(_texture), infPlane(_normal) {
		if (!isNormalized) infPlane.normal.normalize();
		infPlane.offset = _point.dot(infPlane.normal);
	}

	/*Plane(const std::shared_ptr<Texture> &_texture, const Vec3 &a, const Vec3 &b, const Vec3 &c) :
		Object(_texture) {
		infPlane.normal = (b - a).cross(c - b).getNormalized();
		infPlane.offset = a.dot(infPlane.normal);
	}*/

	Plane(const std::shared_ptr<Texture> &_texture, const Vec3 &center, const Vec3 &x, const Vec3 &y) :
		Object(_texture) {
		infPlane.normal = x.cross(y).getNormalized();
		infPlane.offset = center.dot(infPlane.normal);
	}

	Plane(const std::shared_ptr<Texture> &_texture, real_t a, real_t b, real_t c, real_t w) :
		Object(_texture) {
		infPlane.normal = Vec3{ a, b, c }.getNormalized();
		infPlane.offset = -w;
	}

	~Plane() {}

	bool isInfinity() const override {
		return true;
	}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;
};

class PlaneIntersect : public Intersect {
protected:
	const Plane &plane;

	mutable real_t projOrigToInter, projDir;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const override;

	const Object *getObj() const override {
		return &plane;
	}

public:
	PlaneIntersect(const Plane &_plane, const Ray &_ray) : Intersect(_ray), plane(_plane) {}
	~PlaneIntersect() {}

	real_t getDistToInter() const override;
	bool isIntersect() const override;

protected:
	Vec3 getNormal() const override;
};