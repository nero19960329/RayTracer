#include "sphere.h"

#include <gtx/norm.hpp>

Sphere::Sphere(Texture * texture_, glm::dvec3 center_, double radius_, double innerRefrIdx_) :
	Object(texture_), center(center_), radius(radius_), innerRefrIdx(innerRefrIdx_) {}

std::shared_ptr<Intersect> Sphere::getTrace(const Ray & ray, double dist) const {
	auto res = std::make_shared<SphereIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

AABB Sphere::getAABB() const {
	if (aabb.bounds[1] != -max_vec3) return aabb;
	glm::dvec3 radiusVec(radius, radius, radius);
	aabb = AABB(center - radiusVec, center + radiusVec);
	return aabb;
}

std::shared_ptr<Surface> SphereIntersect::getInterPointSurfaceProp() const {
	if (std::abs(interPoint.x) < eps && std::abs(interPoint.y) < eps) {
		return sphere.getTexture()->getProp(0, 0);
	} else {
		double u, v;
		u = std::atan2(normal.y, normal.x) / (PI + PI);
		// cuz (u+1.0)-floor(u+1.0) = u-floor(u), so omit it
		if ((normal.x < 0) ^ (normal.y > 0)) u += 0.5;
		v = std::asin(normal.z) / (PI + PI);
		return sphere.getTexture()->getProp(u, v);
	}
}

double SphereIntersect::getDistToInter() const {
	if (std::isfinite(distToInter)) return distToInter;

	if (!inside) distToInter = distToProjCenter - std::sqrt(halfCord2);
	else if (toward) distToInter = distToProjCenter + std::sqrt(halfCord2);
	else distToInter = std::sqrt(halfCord2) - distToProjCenter;

	interPoint = Intersect::getIntersection();
	return distToInter;
}

bool SphereIntersect::isIntersect() const {
	double distToCenter2 = glm::length2(ray.ori - sphere.center);
	inside = distToCenter2 < (sphere.radius * sphere.radius);

	distToProjCenter = glm::dot(sphere.center - ray.ori, ray.dir);
	toward = distToProjCenter > 0;

	if (!inside && !toward) return false;

	halfCord2 = sphere.radius * sphere.radius - distToCenter2 + distToProjCenter * distToProjCenter;
	if (!inside && halfCord2 < 0) return false;
	return true;
}

glm::dvec3 SphereIntersect::getNormal() const {
	if (!inside) normal = glm::normalize(interPoint - sphere.center);
	else normal = glm::normalize(sphere.center - interPoint);
	return normal;
}

double SphereIntersect::getNextRefrIdx() const {
	if (inside) return VACUUM_REFRACTION_INDEX;
	else return sphere.innerRefrIdx;
}
