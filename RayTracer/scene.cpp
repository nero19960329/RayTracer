#include "constants.h"
#include "scene.h"
#include "utils.h"

bool Scene::isIntersect(const Ray & ray, double dist) const {
	for (const auto & obj : objs) {
		auto intersect = obj->getTrace(ray, dist);
		if (intersect) return true;
	}
	return false;
}

std::shared_ptr<Intersect> Scene::getIntersect(const Ray & ray) const {
	std::shared_ptr<Intersect> res = nullptr;
	double minDist = std::numeric_limits<double>::max();

	for (const auto & obj : objs) {
		auto intersect = obj->getTrace(ray);
		if (intersect) {
			double dis = intersect->getDistToInter();
			if (updateMin(minDist, dis)) res = intersect;
		}
	}

	for (const auto & light : lights) {
		auto intersect = light->getTrace(ray);
		if (intersect) {
			double dis = intersect->getDistToInter();
			if (updateMin(minDist, dis)) res = intersect;
		}
	}

	return res;
}

bool Scene::isInnerPoint(const glm::dvec3 & p) const {
	glm::dvec3 dir(0.0, 1.0, 0.0);
	Ray testRay(p, dir);
	auto intersect = getIntersect(testRay);
	if (!intersect) return true;
	return intersect->hasInside();
}

AABB Scene::getAABB() const {
	if (objs.size() != 1 || (*objs.begin())->isInfinity())
		return { -inf_vec3, inf_vec3 };
	return (*objs.begin())->getAABB();
}
