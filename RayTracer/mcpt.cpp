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
	double pdf;
	if (scene.lights.size() == 1)
		pdf = scene.lights[0]->luminaireSample(*rng, info.interPoint, outDir);
	else exit(0);

	double cos_theta = glm::dot(outDir, info.normal);
	if (cos_theta <= 0.0) return zero_vec3;

	DistRay newRay(Ray{ info.interPoint + outDir * eps, outDir }, ray.dist);
	auto newIntersect = scene.getIntersect(newRay);
	if (newIntersect && !scene.lights[0]->equals(newIntersect->getObj())) return zero_vec3;

	if (!newIntersect) {	// 点光源
		return zero_vec3;
	} else {	//面光源
		BRDFSampleInfo sampleInfo(*rng, inDir, outDir, info.normal, info.material);
		IntersectInfo newInfo = newIntersect->getIntersectInfo();
		double r2 = glm::length2(info.interPoint - newInfo.interPoint);
		return newInfo.material->emission * brdf->eval(sampleInfo) * cos_theta * std::max(glm::dot(-outDir, newInfo.normal), 0.0) / (r2 * pdf);
	}
}

glm::dvec3 MonteCarloPathTracing::getIndirectRadiance(DistRay & ray, std::shared_ptr<Intersect> intersect, RNG * rng, int depth) const {
	IntersectInfo info = intersect->getIntersectInfo();

	double p = 1.0;
	if (depth >= minDepth) {
		p = RUSSIAN_ROULETTE_POSSIBILITY;
		if (rng->randomDouble() >= p) return zero_vec3;
	}

	glm::dvec3 inDir = glm::normalize(ray.ori - info.interPoint);
	glm::dvec3 outDir;
	BRDFSampleInfo sampleInfo(*rng, inDir, outDir, info.normal, info.material, ray.refrIdx / info.nextRefrIdx);
	double pdf = brdf->brdfSample(sampleInfo);
	if (pdf == 0.0) return zero_vec3;
	else if (pdf != -1.0) {
		DistRay newRay(Ray(info.interPoint + outDir * eps, outDir), ray.dist);
		auto newIntersect = scene.getIntersect(newRay);

		double cos_theta = glm::dot(outDir, info.normal);
		if (cos_theta <= 0.0) return zero_vec3;

		return cos_theta * brdf->eval(sampleInfo) * getRayRadiance(newRay, newIntersect, rng, depth + 1) / (p * pdf);
	} else {
		glm::dvec3 reflDir = glm::reflect(-inDir, info.normal);
		DistRay reflRay(Ray(info.interPoint + reflDir * eps, reflDir, ray.refrIdx), ray.dist);
		double nnt = ray.refrIdx / info.nextRefrIdx;
		double ddn = glm::dot(ray.dir, info.normal);
		double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);
		if (cos2t < 0.0)	// 全反射
			return getRayRadiance(reflRay, scene.getIntersect(reflRay), rng, depth + 1) / p;

		double a = info.nextRefrIdx - ray.refrIdx, b = info.nextRefrIdx + ray.refrIdx;
		double R0 = a * a / (b * b);
		double c = 1 + ddn;	// different from smallPt
		double Re = R0 + (1 - R0) * pow(c, 5.0);
		double Tr = 1.0 - Re;
		double P = .25 + .5 * Re;
		double RP = Re / P, TP = Tr / (1 - P);

		DistRay refrRay(Ray(info.interPoint + outDir * eps, outDir, info.nextRefrIdx), ray.dist);
		if (depth > 2) {
			if (rng->randomDouble() < P)
				return getRayRadiance(reflRay, scene.getIntersect(reflRay), rng, depth + 1) * RP / p;
			else
				return getRayRadiance(refrRay, scene.getIntersect(refrRay), rng, depth + 1) * TP / p;
		} else {
			return getRayRadiance(reflRay, scene.getIntersect(reflRay), rng, depth + 1) * Re / p +
				getRayRadiance(refrRay, scene.getIntersect(refrRay), rng, depth + 1) * Tr / p;
		}
	}
}
