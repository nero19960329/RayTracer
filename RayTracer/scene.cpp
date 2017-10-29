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

void Scene::computeLightCDF() {
	double sum = 0.0;
	for (const auto & light : lights)
		sum += light->area;
	lightAreaSum = sum;
	lightCDF.clear();
	lightCDF.push_back(0.0);
	for (const auto & light : lights)
		lightCDF.push_back(light->area);
	for (auto i = 1; i < lightCDF.size(); ++i)
		lightCDF[i] += lightCDF[i - 1];
	for (auto & e : lightCDF)
		e /= sum;
}

std::tuple<int, double> Scene::lightIdxSample(RNG & rng) const {
	int idx = rng.randomIdx(lightCDF);
	return std::make_tuple(idx, lightCDF[idx + 1] - lightCDF[idx]);
}