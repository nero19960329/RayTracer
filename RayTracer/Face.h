#pragma once

#include "Object.h"
#include "Tri.h"

#include <algorithm>
#include <array>

class FaceIntersect;
class SceneReader;

class Face : public Object {
	friend class FaceIntersect;
	friend class SceneReader;

protected:
	Tri tri;

	bool smoothShadingFlag;
	std::array<Vec3, 3> normals;

public:
	Face() : Object(nullptr) {}
	Face(const std::shared_ptr<Texture> &_texture, const Vec3 &_a, const Vec3 &_b, const Vec3 &_c, bool _smoothShadingFlag = false, const std::array<Vec3, 3> &_normals = {}) :
		Object(_texture), tri(_a, _b, _c), smoothShadingFlag(_smoothShadingFlag), normals(_normals) {}
	Face(const std::shared_ptr<Texture> &_texture, const Tri &_tri, bool _smoothShadingFlag = false, const std::array<Vec3, 3> &_normals = {}) :
		Object(_texture), tri(_tri), smoothShadingFlag(_smoothShadingFlag), normals(_normals) {}
	~Face() {}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;

	AABB getAABB() const override {
		if (aabb.bounds[1] != -Vec3::max()) return aabb;
		Vec3 minVec, maxVec;
		rep(k, 3) {
			minVec[k] = std::min(std::min(tri.a[k], tri.b[k]), tri.c[k]);
			maxVec[k] = std::max(std::max(tri.a[k], tri.b[k]), tri.c[k]);
		}
		aabb = AABB{ minVec - Vec3::epsilons(), maxVec + Vec3::epsilons() };
		return aabb;
	}
};

class FaceIntersect : public Intersect {
private:
	const Face &face;

	mutable TriIntersectInfo info;
	mutable Vec3 normal;

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
