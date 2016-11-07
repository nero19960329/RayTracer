#pragma once

#include "Object.h"

class TriangleIntersect;
class SceneReader;

class Triangle : public Object {
	friend class TriangleIntersect;
	friend class SceneReader;

protected:
	Vec3 a, b, c;

public:
	Triangle() : Object(nullptr) {}
	Triangle(const std::shared_ptr<Texture> &_texture, const Vec3 &_a, const Vec3 &_b, const Vec3 &_c) :
		Object(_texture), a(_a), b(_b), c(_c) {}
	~Triangle() {}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;
};

class TriangleIntersect : public Intersect {
private:
	Triangle tri;

	mutable Vec3 normal;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const override;

	const Object *getObj() const override {
		return &tri;
	}

public:
	TriangleIntersect(const Triangle &_tri, const Ray &_ray) :
		Intersect(_ray), tri(_tri) {}
	~TriangleIntersect() {}

	real_t getDistToInter() const override;

	bool isIntersect() const override;
	Vec3 getNormal() const override;
};

using Tri = Triangle;
using TriIntersect = TriangleIntersect;