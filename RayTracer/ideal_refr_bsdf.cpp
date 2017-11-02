#include "ideal_refr_bsdf.h"

#include <cmath>

std::shared_ptr<BSDFSampler> IdealRefrBSDF::getSampler(const Ray & ray_, const IntersectInfo & info_) const {
	return std::make_shared<IdealRefrBSDFSampler>(*this, ray_, info_);
}

std::shared_ptr<BSDFSampler> IdealRefrBSDF::getSampler(const glm::dvec3 & inDir, const glm::dvec3 & outDir, const glm::dvec3 & normal, double inRefr, double outRefr) const {
	Ray ray(zero_vec3, -inDir, inRefr);
	IntersectInfo info;
	info.interPoint = zero_vec3;
	info.normal = normal;
	info.material = nullptr;
	info.nextRefrIdx = outRefr;
	auto res = std::make_shared<IdealRefrBSDFSampler>(*this, ray, info);
	res->setOutDir(outDir);
	return res;
}

Ray IdealRefrBSDFSampler::sample(RNG * rng) const {
	if (refrDir == zero_vec3) {
		outDir = reflDir;
		return Ray(info.interPoint + outDir * eps, outDir, ray.refrIdx);
	}

	double u = rng->randomDouble();
	if (u < P) {
		outDir = reflDir;
		return Ray(info.interPoint + outDir * eps, outDir, ray.refrIdx);
	} else {
		outDir = refrDir;
		return Ray(info.interPoint + outDir * eps, outDir, info.nextRefrIdx);
	}
}

double IdealRefrBSDFSampler::pdf() const {
	if (refrDir == zero_vec3) return 1.0;
	else if (outDir == refrDir) return 1.0 - P;
	else if (outDir == reflDir) return P;
	else return 0.0;
}

double IdealRefrBSDFSampler::pdf_ortho() const {
	return pdf() / std::abs(glm::dot(outDir, info.normal));
}

glm::dvec3 IdealRefrBSDFSampler::eval() const {
	double p = 1.0 / std::abs(glm::dot(outDir, info.normal));
	if (refrDir == zero_vec3)
		return p * one_vec3;
	else if (outDir == refrDir) {
		double scaleFactor = sqr(info.nextRefrIdx / ray.refrIdx);
		return (1.0 - fresnelTerm) * p * one_vec3 * scaleFactor;
	}
	else if (outDir == reflDir && refrDir != zero_vec3)
		return fresnelTerm * p * one_vec3;
	else
		return zero_vec3;
}
