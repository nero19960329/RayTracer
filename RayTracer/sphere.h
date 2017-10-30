#pragma once

#include "constants.h"
#include "object.h"

class SphereIntersect;

class Sphere : public Object {
	friend class SphereIntersect;

protected:
	glm::dvec3 center;
	double radius;
	double innerRefrIdx;

public:
	Sphere(Texture * texture_, glm::dvec3 center_, double radius_, double innerRefrIdx_ = VACUUM_REFRACTION_INDEX);

	double insideRefr() const override { return innerRefrIdx; }
	bool hasInside() const override { return true; }
	std::shared_ptr<Intersect> getTrace(const Ray & ray, double dist = std::numeric_limits<double>::max()) const override;
	AABB getAABB() const override;
};

class SphereIntersect : public Intersect {
private:
	const Sphere & sphere;

	mutable bool toward, inside;
	mutable double distToProjCenter;
	mutable double halfCord2;
	mutable glm::dvec3 interPoint;
	mutable glm::dvec3 normal;

	virtual std::shared_ptr<Material> getInterPointMaterialProp() const override;
	const Object * getObj() const override { return &sphere; }

public:
	SphereIntersect(const Sphere & sphere_, const Ray & ray_) : Intersect(ray_), sphere(sphere_) {}

	double getDistToInter() const override;
	bool isIntersect() const override;
	glm::dvec3 getNormal() const override;
	double getNextRefrIdx() const override;
};