#pragma once

#include "Object.h"
#include "Vec3.h"

class PlaneIntersect;
class SceneReader;

class Plane : public Object {
	friend class PlaneIntersect;
	friend class SceneReader;

protected:
	Vec3 normal;
	real_t offset;

public:
	Plane() : Object(nullptr) {}

	Plane(const std::shared_ptr<Texture> &_texture, const Vec3 &_normal, real_t _offset, bool isNormalized = true) :
		Object(_texture), normal(_normal), offset(_offset) {
		if (!isNormalized) {
			normal.normalize();
		}
	}

	Plane(const std::shared_ptr<Texture> &_texture, const Vec3 &_normal, const Vec3 &_point, bool isNormalized = true) :
		Object(_texture), normal(_normal) {
		if (!isNormalized) {
			normal.normalize();
		}
		offset = _point.dot(normal);
	}

	Plane(const std::shared_ptr<Texture> &_texture, const Vec3 &a, const Vec3 &b, const Vec3 &c) :
		Object(_texture) {
		normal = (b - a).cross(c - b).getNormalized();
		offset = a.dot(normal);
	}

	Plane(const std::shared_ptr<Texture> &_texture, real_t a, real_t b, real_t c, real_t w) :
		Object(_texture) {
		normal = Vec3{ a, b, c }.getNormalized();
		offset = -w;
	}

	~Plane() {}

	bool isInfinity() const override {
		return true;
	}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;
};

class PlaneIntersect : public Intersect {
private:
	Plane plane;

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

private:
	Vec3 getNormal() const override;
};