#pragma once

#include "Face.h"
#include "KDTree.h"
#include "Object.h"
#include "Tri.h"
#include "Vec3.h"

#include <array>
#include <vector>

class MeshIntersect;

class Mesh : public Object {
	friend class MeshIntersect;

protected:
	std::shared_ptr<KDTree> tree;
	std::vector<std::shared_ptr<Object>> faces;

public:
	Mesh() : Object(nullptr) {}
	Mesh(const std::shared_ptr<Texture> &_texture, const std::vector<Tri> &_tris, const std::vector<std::array<int, 3>> &_triIndices, const std::vector<Vec3> &_normals) :
		Object(_texture) {
		if (_normals.size()) {
			rep(i, _tris.size()) {
				std::array<Vec3, 3> vertexNormals = { _normals[_triIndices[i][0]], _normals[_triIndices[i][1]], _normals[_triIndices[i][2]] };
				faces.push_back(std::make_shared<Face>(nullptr, _tris[i], true, vertexNormals));
			}
		} else {
			for (const auto &tri : _tris) faces.push_back(std::make_shared<Face>(nullptr, tri));
		}
		tree = std::make_shared<KDTree>(faces);
	}
	~Mesh() {}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;

	AABB getAABB() const {
		return tree->getAABB();
	}
};

class MeshIntersect : public Intersect {
private:
	const Mesh &mesh;
	std::shared_ptr<KDTreeIntersect> intersect;

	mutable std::vector<Tri>::size_type intersectFaceIdx;
	mutable TriIntersectInfo info;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const override;

	const Object *getObj() const override { return &mesh; }

public:
	MeshIntersect(const Mesh &_mesh, const Ray &_ray) : Intersect(_ray), mesh(_mesh) {
		intersect = std::make_shared<KDTreeIntersect>(*mesh.tree, ray);
	}

	~MeshIntersect() {}

	real_t getDistToInter() const override;
	bool isIntersect() const override;

private:
	Vec3 getNormal() const override;
};