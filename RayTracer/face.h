#pragma once

#include "constants.h"
#include "object.h"

class FaceIntersect;

class Face : public Object {
	friend class FaceIntersect;

protected:
	glm::dvec3 a, b, c;
	glm::dvec3 normals[3];

public:
	Face() : Object(nullptr) {}
	Face(Texture * texture_, glm::dvec3 a_, glm::dvec3 b_, glm::dvec3 c_, glm::dvec3 na = zero_vec3, glm::dvec3 nb = zero_vec3, glm::dvec3 nc = zero_vec3)  :
		Object(texture_), a(a_), b(b_), c(c_) {
		normals[0] = na;
		normals[1] = nb;
		normals[2] = nc;
	}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, double dist = std::numeric_limits<double>::max()) const override;
	AABB getAABB() const override;
	bool liesInPlane(const SplitPlane &p) const override;
};

class FaceIntersect : public Intersect {
private:
	const Face &face;
	mutable double u, v;
	mutable glm::dvec3 normal;

	virtual std::shared_ptr<Material> getInterPointMaterialProp() const override;
	const Object *getObj() const override { return &face; }

public:
	FaceIntersect(const Face &face_, const Ray &ray_) :
		Intersect(ray_), face(face_) {}

	double getDistToInter() const override;
	bool isIntersect() const override;
	glm::dvec3 getNormal() const override;
};