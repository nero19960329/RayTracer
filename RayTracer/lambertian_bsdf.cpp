#include "lambertian_bsdf.h"

std::shared_ptr<BSDFSampler> LambertianBSDF::getSampler(const Ray & ray_, const IntersectInfo & info_) const {
	return std::make_shared<LambertianBSDFSampler>(*this, ray_, info_);
}

std::shared_ptr<BSDFSampler> LambertianBSDF::getSampler(const glm::dvec3 & inDir, const glm::dvec3 & outDir, const glm::dvec3 & normal, double inRefr, double outRefr) const {
	Ray ray(zero_vec3, -inDir, inRefr);
	IntersectInfo info;
	info.interPoint = zero_vec3;
	info.normal = normal;
	info.material = nullptr;
	info.nextRefrIdx = outRefr;
	auto res = std::make_shared<LambertianBSDFSampler>(*this, ray, info);
	res->setOutDir(outDir);
	return res;
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
	return glm::dot(outDir, info.normal) * pdf_ortho();
}

double LambertianBSDFSampler::pdf_ortho() const {
	return INV_PI;
}

glm::dvec3 LambertianBSDFSampler::eval() const {
	return bsdf.diffuse * INV_PI;
}