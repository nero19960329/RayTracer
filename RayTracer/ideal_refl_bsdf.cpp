#include "ideal_refl_bsdf.h"

std::shared_ptr<BSDFSampler> IdealReflBSDF::getSampler(const Ray & ray_, const IntersectInfo & info_) const {
	return std::make_shared<IdealReflBSDFSampler>(*this, ray_, info_);
}

Ray IdealReflBSDFSampler::sample(RNG * rng) const {
	outDir = reflDir;
	return Ray(info.interPoint + outDir * eps, outDir, ray.refrIdx);
}

double IdealReflBSDFSampler::pdf() const {
	if (outDir == reflDir) return 1.0;
	else return 0.0;
}

glm::dvec3 IdealReflBSDFSampler::eval() const {
	if (outDir == reflDir)
		return bsdf.k_s * one_vec3 / glm::dot(outDir, info.normal);
	else return zero_vec3;
}