#include "light.h"

double PointLight::luminaireSample(RNG & rng, glm::dvec3 interPoint, glm::dvec3 & outDir) const {
	outDir = glm::normalize(center - interPoint);
	return 1.0;
}

std::shared_ptr<Intersect> PointLight::getTrace(const Ray & ray, double dist) const {
	return nullptr;
}

double RectLight::luminaireSample(RNG & rng, glm::dvec3 interPoint, glm::dvec3 & outDir) const {
	glm::dvec3 u = (rng.randomDouble() - 0.5) * x, v = (rng.randomDouble() - 0.5) * y;
	glm::dvec3 target = center + u + v;
	outDir = glm::normalize(target - interPoint);
	return area;
}

std::shared_ptr<Intersect> RectLight::getTrace(const Ray & ray, double dist) const {
	auto intersect = f1.getTrace(ray, dist);
	if (intersect) return intersect;
	intersect = f2.getTrace(ray, dist);
	if (intersect) return intersect;
	else return nullptr;
}
