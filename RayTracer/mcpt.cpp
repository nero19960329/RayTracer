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

glm::dvec3 MonteCarloPathTracing::getRayRadiance(DistRay & ray, std::shared_ptr<Intersect> intersect, RNG * rng, int depth) const {
	if (!intersect) return zero_vec3;
	
	IntersectInfo info = intersect->getIntersectInfo();
	glm::dvec3 emittance = info.material->emission;
	glm::dvec3 reflectance = getDirectRadiance(ray, intersect, rng) + getIndirectRadiance(ray, intersect, rng, depth);
	return emittance + reflectance;
}

glm::dvec3 MonteCarloPathTracing::getDirectRadiance(DistRay & ray, std::shared_ptr<Intersect> intersect, RNG * rng) const {
	IntersectInfo info = intersect->getIntersectInfo();

	glm::dvec3 inDir = glm::normalize(ray.ori - info.interPoint);
	glm::dvec3 outDir;

	auto &[lightIdx, ratio] = scene.lightIdxSample(*rng);
	auto sampleLight = scene.lights[lightIdx];
	sampleLight->luminaireSample(*rng, info.interPoint, outDir);
	double pdf = ratio * sampleLight->pdf(*rng, info.interPoint, outDir);
	if (pdf == 0.0) return zero_vec3;

	double cos_theta = glm::dot(outDir, info.normal);
	if (cos_theta <= 0.0) return zero_vec3;

	DistRay newRay(Ray{ info.interPoint + outDir * eps, outDir }, ray.dist);
	auto newIntersect = scene.getIntersect(newRay);
	if (!newIntersect || (newIntersect && !sampleLight->equals(newIntersect->getObj()))) return zero_vec3;

	BRDFSampleInfo sampleInfo(*rng, inDir, outDir, info.normal, info.material, ray.refrIdx, ray.refrIdx);
	IntersectInfo newInfo = newIntersect->getIntersectInfo();
	double r2 = glm::length2(info.interPoint - newInfo.interPoint);
	return newInfo.material->emission * brdf->eval(sampleInfo) * cos_theta * std::max(glm::dot(-outDir, newInfo.normal), 0.0) / (r2 * pdf);
}

glm::dvec3 MonteCarloPathTracing::getIndirectRadiance(DistRay & ray, std::shared_ptr<Intersect> intersect, RNG * rng, int depth) const {
	double p = 1.0;
	if (depth >= minDepth) {
		p = RUSSIAN_ROULETTE_POSSIBILITY;
		if (rng->randomDouble() >= p) return zero_vec3;
	}

	IntersectInfo info = intersect->getIntersectInfo();
	glm::dvec3 inDir = glm::normalize(ray.ori - info.interPoint);
	glm::dvec3 outDir;
	BRDFSampleInfo sampleInfo(*rng, inDir, outDir, info.normal, info.material, ray.refrIdx, info.nextRefrIdx);
	brdf->brdfSample(sampleInfo);
	double pdf = brdf->pdf(sampleInfo);
	if (pdf == 0.0) return zero_vec3;
	else {
		DistRay newRay(Ray(info.interPoint + outDir * eps, outDir, sampleInfo.nextRefr), ray.dist);
		auto newIntersect = scene.getIntersect(newRay);
		return glm::dot(outDir, info.normal) * brdf->eval(sampleInfo) * getRayRadiance(newRay, newIntersect, rng, depth + 1) / (p * pdf);
	}
}
