#pragma once

#include "object.h"

class PlaneIntersect;

class Plane : public Object {
	friend class PlaneIntersect;

protected:
	glm::dvec3 normal;
	double offset;

public:
	Plane(Texture * texture_, glm::dvec3 normal_, double offset_);
	Plane(Texture * texture_, glm::dvec3 normal_, glm::dvec3 point);

	bool isInfinity() const override;
	std::shared_ptr<Intersect> getTrace(const Ray & ray, double dist = std::numeric_limits<double>::max()) const override;
};

class PlaneIntersect : public Intersect {
protected:
	const Plane & plane;

	mutable double projOrigToInter, projDir;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProp() const override;
	const Object * getObj() const override;

public:
	PlaneIntersect(const Plane & plane_, const Ray & ray_) : Intersect(ray_), plane(plane_) {}

	double getDistToInter() const override;
	bool isIntersect() const override;
	glm::dvec3 getNormal() const override;
};