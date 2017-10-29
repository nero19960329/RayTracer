#include "mcpt.h"

#include <gtx/norm.hpp>

glm::dvec3 MonteCarloPathTracing::color(const Ray & ray, RNG * rng) const {
	DistRay distRay(ray, 0.0);
	return getColor(distRay, rng);
}

glm::dvec3 MonteCarloPathTracing::getColor(DistRay & ray, RNG * rng, int depth) const {
	auto intersect = scene.getIntersect(ray);
	return getRayRadiance(ray, intersect, rng);
}

glm::dvec3 MonteCarloPathTracing::getRayRadiance(DistRay & ray, std::shared_ptr<Intersect> intersect, RNG * rng) const {
	if (!intersect) return zero_vec3;
	IntersectInfo info = intersect->getIntersectInfo();
	glm::dvec3 emittance = info.material->emission;
	glm::dvec3 reflectance = getReflectedRadiance(ray, intersect, rng);
	return emittance + reflectance;
}

glm::dvec3 MonteCarloPathTracing::getReflectedRadiance(DistRay & ray, std::shared_ptr<Intersect> intersect, RNG * rng, int depth) const {
	if (!intersect) return zero_vec3;

	IntersectInfo info = intersect->getIntersectInfo();
	glm::dvec3 reflectedRadiance = zero_vec3;

	glm::dvec3 inDir = glm::normalize(ray.ori - info.interPoint);
	glm::dvec3 outDir;

	// Direct Radiance
	// Multiple Importance Sampling
	auto &[lightIdx, ratio] = scene.lightIdxSample(*rng);
	auto sampleLight = scene.lights[lightIdx];

	sampleLight->luminaireSample(*rng, info.interPoint, outDir);
	double pdf1 = ratio * sampleLight->pdf(*rng);
	double pdf2;

	BRDFSampleInfo sampleInfo(*rng, inDir, outDir, info.normal, info.material, ray.refrIdx, ray.refrIdx);

	DistRay newRay(Ray{ info.interPoint + outDir * eps, outDir }, ray.dist);
	auto newIntersect = scene.getIntersect(newRay);
	if (newIntersect && sampleLight->equals(newIntersect->getObj())) {
		IntersectInfo newInfo = newIntersect->getIntersectInfo();
		pdf1 *= glm::length2(info.interPoint - newInfo.interPoint) / glm::dot(-outDir, newInfo.normal);
		pdf2 = brdf->pdf(sampleInfo);
		reflectedRadiance += newInfo.material->emission * brdf->eval(sampleInfo) * glm::dot(outDir, info.normal) / (pdf1 + pdf2);
	}

	sampleInfo.nextRefr = info.nextRefrIdx;
	brdf->brdfSample(sampleInfo);
	newRay = DistRay(Ray{ info.interPoint + outDir * eps, outDir, sampleInfo.nextRefr }, ray.dist);
	newIntersect = scene.getIntersect(newRay);
	if (newIntersect && newIntersect->getObj()->isLight()) {
		IntersectInfo newInfo = newIntersect->getIntersectInfo();
		ratio = ((Light *)(newIntersect->getObj()))->area / scene.lightAreaSum;
		pdf1 = ratio * sampleLight->pdf(*rng) * glm::length2(info.interPoint - newInfo.interPoint) / glm::dot(-outDir, newInfo.normal);
		pdf2 = brdf->pdf(sampleInfo);
		reflectedRadiance += newInfo.material->emission * brdf->eval(sampleInfo) * glm::dot(outDir, info.normal) / (pdf1 + pdf2);
	}

	// Indirect Radiance
	double p = 1.0;
	if (depth >= minDepth) {
		p = RUSSIAN_ROULETTE_POSSIBILITY;
		if (rng->randomDouble() >= p) return reflectedRadiance;
	}

	double pdf = brdf->pdf(sampleInfo);
	if (pdf != 0.0)
		reflectedRadiance += glm::dot(outDir, info.normal) * brdf->eval(sampleInfo) * getReflectedRadiance(newRay, newIntersect, rng, depth + 1) / (p * pdf);
	return reflectedRadiance;
}