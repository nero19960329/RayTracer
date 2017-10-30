#include <algorithm>

#include "face.h"

std::shared_ptr<Intersect> Face::getTrace(const Ray & ray, double dist) const {
	auto res = std::make_shared<FaceIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

AABB Face::getAABB() const {
	if (aabb.bounds[1] != -max_vec3) return aabb;
	glm::dvec3 minVec, maxVec;
	for (int k = 0; k < 3; ++k) {
		minVec[k] = std::min(std::min(a[k], b[k]), c[k]);
		maxVec[k] = std::max(std::max(a[k], b[k]), c[k]);
	}
	aabb = AABB{ minVec - eps_vec3, maxVec + eps_vec3 };
	return aabb;
}

bool Face::liesInPlane(const SplitPlane & p) const {
	return std::abs(a[p.axis] - p.value) < eps &&
		std::abs(b[p.axis] - p.value) < eps &&
		std::abs(c[p.axis] - p.value) < eps;
}

std::shared_ptr<Material> FaceIntersect::getInterPointMaterialProp() const {
	return nullptr;
}

double FaceIntersect::getDistToInter() const {
	return distToInter;
}

bool FaceIntersect::isIntersect() const {
	glm::dvec3 e1 = face.b - face.a, e2 = face.c - face.a;
	glm::dvec3 s = ray.ori - face.a;
	glm::dvec3 tmpVec1 = glm::cross(ray.dir, e2);
	glm::dvec3 tmpVec2 = glm::cross(s, e1);

	double tmp = glm::dot(tmpVec1, e1);
	if (tmp > eps) {
		u = glm::dot(tmpVec1, s);
		if (u < 0 || u > tmp) return false;
		v = glm::dot(tmpVec2, ray.dir);
		if (v < 0 || u + v > tmp) return false;
	} else if (tmp < -eps) {
		u = glm::dot(tmpVec1, s);
		if (u > 0 || u < tmp) return false;
		v = glm::dot(tmpVec2, ray.dir);
		if (v > 0 || u + v < tmp) return false;
	} else return false;

	distToInter = glm::dot(tmpVec2, e2);
	if ((distToInter < 0) ^ (tmp < 0)) return false;
	double f = 1.0 / tmp;

	distToInter *= f;  u *= f; v *= f;
	return true;
}

glm::dvec3 FaceIntersect::getNormal() const {
	glm::dvec3 normal;
	if (face.normals[0] == zero_vec3 && face.normals[1] == zero_vec3 && face.normals[2] == zero_vec3)
		normal = glm::normalize(glm::cross(face.b - face.a, face.c - face.b));
	else
		normal = (1 - u - v) * face.normals[0] + u * face.normals[1] + v * face.normals[2];
	return normal;
}
