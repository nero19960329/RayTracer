#include "ideal_refl_bsdf.h"

std::shared_ptr<BSDFSampler> IdealReflBSDF::getSampler(const Ray & ray_, const IntersectInfo & info_) const {
	return std::make_shared<IdealReflBSDFSampler>(*this, ray_, info_);
}

std::shared_ptr<BSDFSampler> IdealReflBSDF::getSampler(const glm::dvec3 & inDir, const glm::dvec3 & outDir, const glm::dvec3 & normal, double inRefr, double outRefr) const {
	Ray ray(zero_vec3, -inDir, inRefr);
	IntersectInfo info;
	info.interPoint = zero_vec3;
	info.normal = normal;
	info.material = nullptr;
	info.nextRefrIdx = outRefr;
	auto res = std::make_shared<IdealReflBSDFSampler>(*this, ray, info);
	res->setOutDir(outDir);
	return res;
}

Ray IdealReflBSDFSampler::sample(RNG * rng) const {
	outDir = reflDir;
	return Ray(info.interPoint + outDir * eps, outDir, ray.refrIdx);
}

double IdealReflBSDFSampler::pdf() const {
	if (outDir == reflDir) return 1.0;
	else return 0.0;
}

double IdealReflBSDFSampler::pdf_ortho() const {
	return pdf() / glm::dot(outDir, info.normal);
}

glm::dvec3 IdealReflBSDFSampler::eval() const {
	if (outDir == reflDir)
		return bsdf.k_s * one_vec3 / glm::dot(outDir, info.normal);
	else return zero_vec3;
}