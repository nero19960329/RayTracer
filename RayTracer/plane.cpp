#include "plane.h"

Plane::Plane(Texture * texture_, glm::dvec3 normal_, double offset_) : 
	Object(texture_), normal(normal_), offset(offset_) {
	normal = glm::normalize(normal_);
}

Plane::Plane(Texture * texture_, glm::dvec3 normal_, glm::dvec3 point) :
	Object(texture_), normal(normal_) {
	normal = glm::normalize(normal_);
	offset = glm::dot(point, normal);
}

bool Plane::isInfinity() const {
	return true;
}

std::shared_ptr<Intersect> Plane::getTrace(const Ray & ray, double dist) const {
	auto res = std::make_shared<PlaneIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

std::shared_ptr<Material> PlaneIntersect::getInterPointMaterialProp() const {
	glm::dvec3 x(-plane.normal.y, plane.normal.x, 0);
	glm::dvec3 y(0, -plane.normal.z, plane.normal.y);
	if (glm::length(x) < eps) x = glm::dvec3(-plane.normal.z, 0, plane.normal.x);
	if (glm::length(y) < eps) y = glm::dvec3(-plane.normal.z, 0, plane.normal.x);

	return plane.getTexture()->getProp(glm::dot(interPoint, x), glm::dot(interPoint, y));
}

const Object * PlaneIntersect::getObj() const {
	return &plane;
}

double PlaneIntersect::getDistToInter() const {
	distToInter = -projOrigToInter / projDir;
	return distToInter;
}

bool PlaneIntersect::isIntersect() const {
	projDir = glm::dot(ray.dir, plane.normal);
	if (std::abs(projDir) < eps) return false;
	else {
		projOrigToInter = glm::dot(ray.ori, plane.normal) - plane.offset;
		if ((projOrigToInter > 0) ^ (projDir > 0)) return true;
		else return false;
	}
}

glm::dvec3 PlaneIntersect::getNormal() const {
	if (glm::dot(ray.dir, plane.normal) < 0) return plane.normal;
	else return -plane.normal;
}
