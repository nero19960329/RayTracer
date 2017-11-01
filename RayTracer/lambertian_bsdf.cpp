#include "lambertian_bsdf.h"

std::shared_ptr<BSDFSampler> LambertianBSDF::getSampler(const Ray & ray_, const IntersectInfo & info_) const {
	return std::make_shared<LambertianBSDFSampler>(*this, ray_, info_);
}

Ray LambertianBSDFSampler::sample(RNG * rng) const {
	double u = rng->randomDouble();
	if (u < bsdf.k_d) {
		double theta = std::acos(sqrt(rng->randomDouble()));
		double phi = 2.0 * PI * rng->randomDouble();
		outDir = change2World(info.normal, theta, phi);
	} else outDir = zero_vec3;
	return Ray(info.interPoint + outDir * eps, outDir, ray.refrIdx);
}

double LambertianBSDFSampler::pdf() const {
	return glm::dot(outDir, info.normal) * INV_PI;
}

glm::dvec3 LambertianBSDFSampler::eval() const {
	return bsdf.diffuse * INV_PI;
}