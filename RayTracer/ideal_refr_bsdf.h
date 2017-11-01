#pragma once

#include "bsdf.h"
#include "utils.h"

class IdealRefrBSDFSampler;

class IdealRefrBSDF : public BSDF {
public:
	IdealRefrBSDF() {}

	std::shared_ptr<BSDF> clone() const override { return std::make_shared<IdealRefrBSDF>(*this); }

	std::shared_ptr<BSDFSampler> getSampler(const Ray & ray_, const IntersectInfo & info_) const;
};

class IdealRefrBSDFSampler : public BSDFSampler {
private:
	const IdealRefrBSDF & bsdf;
	mutable double fresnelTerm, P;
	mutable glm::dvec3 reflDir, refrDir;

public:
	IdealRefrBSDFSampler(const IdealRefrBSDF & bsdf_, const Ray & ray_, const IntersectInfo & info_) :
		BSDFSampler(ray_, info_), bsdf(bsdf_) {
		double eta1 = ray.refrIdx;
		double eta2 = info.nextRefrIdx;
		reflDir = glm::reflect(ray.dir, info.normal);
		refrDir = glm::refract(ray.dir, info.normal, eta1 / eta2);

		double cos_theta1 = glm::dot(-ray.dir, info.normal);
		double cos_theta2 = std::sqrt(1.0 - sqr(eta1 / eta2) * (1.0 - sqr(cos_theta1)));

		double Rs = (eta1 * cos_theta1 - eta2 * cos_theta2) / (eta2 * cos_theta1 + eta1 * cos_theta2);
		double Rp = (eta2 * cos_theta1 - eta1 * cos_theta2) / (eta2 * cos_theta1 + eta1 * cos_theta2);
		double Re = 0.5 * (sqr(Rs) + sqr(Rp));
		fresnelTerm = Re;
		P = 0.25 + 0.5 * Re;
	}

	Ray sample(RNG * rng) const override;
	double pdf() const override;
	glm::dvec3 eval() const override;
};