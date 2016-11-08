#pragma once

#include "Object.h"
#include "Tri.h"
#include "Vec3.h"

#include <array>
#include <vector>

class MeshIntersect;
class SceneReader;

class Mesh : public Object {
	friend class MeshIntersect;
	friend class SceneReader;

protected:
	std::vector<Tri> tris;
	std::vector<std::array<int, 3>> triIndices;
	std::vector<Vec3> normals;

public:
	Mesh() : Object(nullptr) {}
	Mesh(const std::shared_ptr<Texture> &_texture, const std::vector<Tri> &_tris, const std::vector<std::array<int, 3>> &_triIndices, const std::vector<Vec3> &_normals) :
		Object(_texture), tris(_tris), triIndices(_triIndices), normals(_normals) {}
	~Mesh() {}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;
};

class MeshIntersect : public Intersect {
private:
	Mesh mesh;

	mutable std::vector<Tri>::size_type intersectFaceIdx;
	mutable TriIntersectInfo info;

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