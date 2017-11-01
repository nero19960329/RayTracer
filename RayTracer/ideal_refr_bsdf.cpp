#include "ideal_refr_bsdf.h"

std::shared_ptr<BSDFSampler> IdealRefrBSDF::getSampler(const Ray & ray_, const IntersectInfo & info_) const {
	return std::make_shared<IdealRefrBSDFSampler>(*this, ray_, info_);
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

glm::dvec3 IdealRefrBSDFSampler::eval() const {
	double p = 1.0 / std::abs(glm::dot(outDir, info.normal));
	if (refrDir == zero_vec3) return p * one_vec3;
	else if (outDir == refrDir) {
		double scaleFactor = sqr(info.nextRefrIdx / ray.refrIdx);
		return (1.0 - fresnelTerm) * p * one_vec3 * scaleFactor;
	}
	else if (outDir == reflDir && refrDir != zero_vec3) return fresnelTerm * p * one_vec3;
	else return zero_vec3;
}
