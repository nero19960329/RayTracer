#pragma once

#include "Object.h"
#include "Triangle.h"
#include "Vec3.h"

#include <vector>

class MeshIntersect;
class SceneReader;

class Mesh : public Object {
	friend class MeshIntersect;
	friend class SceneReader;

protected:
	std::vector<Tri> tris;

public:
	Mesh() : Object(nullptr) {}
	Mesh(const std::shared_ptr<Texture> &_texture, const std::vector<Tri> &_tris) :
		Object(_texture), tris(_tris) {}
	~Mesh() {}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;
};

class MeshIntersect : public Intersect {
private:
	Mesh mesh;

	mutable std::shared_ptr<Intersect> nearestIntersect;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const override;

	const Object *getObj() const override {
		return &mesh;
	}

public:
	MeshIntersect(const Mesh &_mesh, const Ray &_ray) : Intersect(_ray), mesh(_mesh) {}
	~MeshIntersect() {}

	real_t getDistToInter() const override;
	bool isIntersect() const override;

private:
	Vec3 getNormal() const override;
};