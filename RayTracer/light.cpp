#include "light.h"

glm::dvec3 PointLight::getNormal(glm::dvec3 p) const { return zero_vec3; }

glm::dvec3 PointLight::luminaireSample(RNG & rng) const { return zero_vec3; }

void PointLight::luminaireSample(RNG & rng, glm::dvec3 interPoint, glm::dvec3 & outDir) const {}

double PointLight::pdf(RNG & rng) const {
	return 0.0;
}

std::shared_ptr<Intersect> PointLight::getTrace(const Ray & ray, double dist) const {
	return nullptr;
}

glm::dvec3 RectLight::getNormal(glm::dvec3 p) const {
	return n;
}

glm::dvec3 RectLight::luminaireSample(RNG & rng) const {
	glm::dvec3 u = (rng.randomDouble() - 0.5) * x, v = (rng.randomDouble() - 0.5) * y;
	glm::dvec3 target = center + u + v;
	return target;
}

void RectLight::luminaireSample(RNG & rng, glm::dvec3 interPoint, glm::dvec3 & outDir) const {
	glm::dvec3 target = luminaireSample(rng);
	outDir = glm::normalize(target - interPoint);
}

double RectLight::pdf(RNG & rng) const {
	return 1.0 / area;
}

std::shared_ptr<Intersect> RectLight::getTrace(const Ray & ray, double dist) const {
	auto intersect = f1.getTrace(ray, dist);
	if (intersect) return intersect;
	intersect = f2.getTrace(ray, dist);
	if (intersect) return intersect;
	else return nullptr;
}
