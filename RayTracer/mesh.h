#pragma once

#include "face.h"
#include "kdtree.h"
#include "tiny_obj_loader.h"

#include <vector>

class MeshIntersect;

class Mesh : public Object {
	friend class MeshIntersect;

protected:
	KDTree * tree;
	std::vector<Object *> faces;

private:
	tinyobj::shape_t shape;
	tinyobj::material_t material;
	tinyobj::attrib_t attrib;

public:
	Mesh(Texture * texture_, const std::string & filename);
	Mesh(Texture * texture_, const tinyobj::shape_t & shape_, const tinyobj::material_t & material_, const tinyobj::attrib_t & attrib_);
	Mesh(const tinyobj::shape_t & shape_, const tinyobj::material_t & material_, const tinyobj::attrib_t & attrib_);

	std::shared_ptr<Intersect> getTrace(const Ray & ray, double dist = std::numeric_limits<double>::max()) const override;
	AABB getAABB() const;
	bool hasInside() const override;

	void scaleToBoundingSphere(glm::dvec3 center, double radius);
	void update();
};

class MeshIntersect : public Intersect {
private:
	const Mesh & mesh;
	std::shared_ptr<KDTreeIntersect> intersect;

	mutable bool inside;
	mutable int intersectFaceIdx;

	virtual std::shared_ptr<Material> getInterPointMaterialProp() const override;
	const Object * getObj() const override;

public:
	MeshIntersect(const Mesh & mesh_, const Ray & ray_);

	double getDistToInter() const override;
	bool isIntersect() const override;
	glm::dvec3 getNormal() const override;
};