#include "bsdf.h"
#include "mcpt.h"

#include <gtx/norm.hpp>

glm::dvec3 MonteCarloPathTracing::color(const Ray & ray, RNG * rng) const {
	return getColor(Ray(ray), rng);
}

glm::dvec3 MonteCarloPathTracing::getColor(Ray & ray, RNG * rng, int depth) const {
	auto intersect = scene.getIntersect(ray);
	return getRayRadiance(ray, intersect, rng);
}

glm::dvec3 MonteCarloPathTracing::getRayRadiance(Ray & ray, std::shared_ptr<Intersect> intersect, RNG * rng) const {
	if (!intersect) return zero_vec3;
	IntersectInfo info = intersect->getIntersectInfo();
	glm::dvec3 emittance = info.material->emission;
	glm::dvec3 reflectance = getReflectedRadiance(ray, intersect, rng);
	return emittance + reflectance;
}

glm::dvec3 MonteCarloPathTracing::getReflectedRadiance(Ray & ray, std::shared_ptr<Intersect> intersect, RNG * rng, int depth) const {
	if (!intersect) return zero_vec3;

	IntersectInfo info = intersect->getIntersectInfo();
	glm::dvec3 reflectedRadiance = zero_vec3;

	glm::dvec3 outDir;

	// Direct Radiance
	// Multiple Importance Sampling
	auto &[lightIdx, ratio] = scene.lightIdxSample(*rng);
	auto sampleLight = scene.lights[lightIdx];

	sampleLight->luminaireSample(*rng, info.interPoint, outDir);
	double pdf1 = ratio * sampleLight->pdf(*rng);
	double pdf2;

	Ray newRay(info.interPoint + outDir * eps, outDir);
	auto newIntersect = scene.getIntersect(newRay);
	auto sampler = info.material->bsdf->getSampler(ray, info);
	sampler->setOutDir(outDir);
	if (newIntersect && sampleLight->equals(newIntersect->getObj()) && glm::dot(-outDir, newIntersect->getIntersectInfo().normal) > 0.0) {
		IntersectInfo newInfo = newIntersect->getIntersectInfo();
		pdf1 *= glm::length2(info.interPoint - newInfo.interPoint) / glm::dot(-outDir, newInfo.normal);
		pdf2 = sampler->pdf();
		reflectedRadiance += newInfo.material->emission * sampler->eval() * glm::dot(outDir, info.normal) / (pdf1 + pdf2);
	}

	newRay = sampler->sample(rng);
	outDir = newRay.dir;
	newIntersect = scene.getIntersect(newRay);
	if (newIntersect && newIntersect->getObj()->isLight() && glm::dot(-outDir, newIntersect->getIntersectInfo().normal) > 0.0) {
		IntersectInfo newInfo = newIntersect->getIntersectInfo();
		ratio = ((Light *)(newIntersect->getObj()))->area / scene.lightAreaSum;
		pdf1 = ratio * sampleLight->pdf(*rng) * glm::length2(info.interPoint - newInfo.interPoint) / glm::dot(-outDir, newInfo.normal);
		pdf2 = sampler->pdf();
		reflectedRadiance += newInfo.material->emission * sampler->eval() * glm::dot(outDir, info.normal) / (pdf1 + pdf2);
	}

	// Indirect Radiance
	double p = 1.0;
	if (depth >= minDepth) {
		p = RUSSIAN_ROULETTE_POSSIBILITY;
		if (rng->randomDouble() >= p) return reflectedRadiance;
	}

	double pdf = sampler->pdf();
	if (pdf != 0.0) {
		glm::dvec3 indirect = getReflectedRadiance(newRay, newIntersect, rng, depth + 1);
		reflectedRadiance += std::abs(glm::dot(outDir, info.normal)) * sampler->eval() * indirect / (p * pdf);
	}

	return reflectedRadiance;
}