#pragma once

#include "bsdf.h"

class LambertianBSDF : public BSDF {
private:
	glm::dvec3 diffuse;
	mutable double k_d;

public:
	LambertianBSDF() {}

	void setColor(glm::dvec3 color) override {
		diffuse = color;
		k_d = std::max(diffuse[0], std::max(diffuse[1], diffuse[2]));
	}

	void bsdfSample(BSDFSampleInfo & info) const override {
		double u = info.rng.randomDouble();
		if (u < k_d) {
			double theta = std::acos(sqrt(info.rng.randomDouble()));
			double phi = 2.0 * PI * info.rng.randomDouble();
			info.outDir = change2World(info.normal, theta, phi);
			info.nextRefr = info.thisRefr;
		} else info.outDir = zero_vec3;
	}

	double pdf(BSDFSampleInfo & info) const override {
		return k_d * glm::dot(info.outDir, info.normal) * INV_PI;
	}

	glm::dvec3 eval(BSDFSampleInfo & info) const override {
		return diffuse * INV_PI;
	}
};