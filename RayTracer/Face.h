#pragma once

#include "Object.h"
#include "Tri.h"

class FaceIntersect;
class SceneReader;

class Face : public Object {
	friend class FaceIntersect;
	friend class SceneReader;

protected:
	Tri tri;

public:
	Face() : Object(nullptr) {}
	Face(const std::shared_ptr<Texture> &_texture, const Vec3 &_a, const Vec3 &_b, const Vec3 &_c) :
		Object(_texture), tri(_a, _b, _c) {}
	Face(const std::shared_ptr<Texture> &_texture, const Tri &_tri) :
		Object(_texture), tri(_tri) {}
	~Face() {}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;

	AABB getAABB() const {
		Vec3 minVec, maxVec;
		rep(k, 3) {
			minVec[k] = min(min(tri.a[k], tri.b[k]), tri.c[k]);
			maxVec[k] = max(max(tri.a[k], tri.b[k]), tri.c[k]);
		}
		return AABB{ minVec - Vec3::epsilons(), maxVec + Vec3::epsilons() };
	}
};

class FaceIntersect : public Intersect {
private:
	const Face &face;

	mutable Vec3 normal;
	mutable real_t u, v;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const override;

	const Object *getObj() const override {
		return &face;
	}

public:
	FaceIntersect(const Face &_face, const Ray &_ray) :
		Intersect(_ray), face(_face) {}
	~FaceIntersect() {}

	real_t getDistToInter() const override;

	bool isIntersect() const override;
	Vec3 getNormal() const override;
};
