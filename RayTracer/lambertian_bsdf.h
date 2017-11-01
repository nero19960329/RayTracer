#pragma once

#include "bsdf.h"

class LambertianBSDFSampler;

class LambertianBSDF : public BSDF {
	friend class LambertianBSDFSampler;

protected:
	glm::dvec3 diffuse;
	mutable double k_d;

public:
	LambertianBSDF() {}

	std::shared_ptr<BSDF> clone() const override { return std::make_shared<LambertianBSDF>(*this); }

	void setColor(glm::dvec3 color) override {
		diffuse = color;
		k_d = std::max(diffuse[0], std::max(diffuse[1], diffuse[2]));
	}

	std::shared_ptr<BSDFSampler> getSampler(const Ray & ray_, const IntersectInfo & info_) const;
};

class LambertianBSDFSampler : public BSDFSampler {
private:
	const LambertianBSDF & bsdf;

public:
	LambertianBSDFSampler(const LambertianBSDF & bsdf_, const Ray & ray_, const IntersectInfo & info_) :
		BSDFSampler(ray_, info_), bsdf(bsdf_) {}

	Ray sample(RNG * rng) const override;
	double pdf() const override;
	glm::dvec3 eval() const override;
};