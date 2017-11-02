#pragma once

#include "bsdf.h"

class IdealReflBSDFSampler;

class IdealReflBSDF : public BSDF {
	friend class IdealReflBSDFSampler;

protected:
	double k_s;

public:
	IdealReflBSDF(double specular) : k_s(specular) {}

	std::shared_ptr<BSDF> clone() const override { return std::make_shared<IdealReflBSDF>(*this); }

	std::shared_ptr<BSDFSampler> getSampler(const Ray & ray_, const IntersectInfo & info_) const;
	std::shared_ptr<BSDFSampler> getSampler(const glm::dvec3 & inDir, const glm::dvec3 & outDir, const glm::dvec3 & normal, double inRefr, double outRefr) const;
};

class IdealReflBSDFSampler : public BSDFSampler {
private:
	const IdealReflBSDF & bsdf;
	mutable glm::dvec3 reflDir;

public:
	IdealReflBSDFSampler(const IdealReflBSDF & bsdf_, const Ray & ray_, const IntersectInfo & info_) :
		BSDFSampler(ray_, info_), bsdf(bsdf_) {
		reflDir = glm::reflect(ray.dir, info.normal);
	}

	Ray sample(RNG * rng) const override;
	double pdf() const override;
	double pdf_ortho() const override;
	glm::dvec3 eval() const override;
};