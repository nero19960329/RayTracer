#pragma once
/*
#include "bsdf.h"

class PhongBSDF : public BSDF {
private:
	double specular, shininess;
	glm::dvec3 diffuse;

	mutable double k_d, k_s;

public:
	PhongBSDF(double specular_, double shininess_) :
		specular(specular_), shininess(shininess_) {
		k_s = specular;
	}

	std::shared_ptr<BSDF> clone() const override { return std::make_shared<PhongBSDF>(*this); }

	void setColor(glm::dvec3 color) override {
		diffuse = color;
		k_d = std::max(diffuse[0], std::max(diffuse[1], diffuse[2]));
	}

	void bsdfSample(BSDFSampleInfo & info) const override {
		double u = info.rng.randomDouble();

		if (u < k_d) {
			double theta = std::asin(std::sqrt(1.0 - info.rng.randomDouble()));
			double phi = 2.0 * PI * info.rng.randomDouble();
			info.outDir = change2World(info.normal, theta, phi);
			info.nextRefr = info.thisRefr;
		} else if (u < k_d + k_s) {
			double theta = std::acos(std::pow(1.0 - info.rng.randomDouble(), 1.0 / (shininess + 1)));
			double phi = 2.0 * PI * info.rng.randomDouble();
			glm::dvec3 ref = glm::reflect(-info.inDir, info.normal);
			info.outDir = change2World(ref, theta, phi);
			if (glm::dot(info.outDir, info.normal) < 0.0) {
				info.outDir = zero_vec3;
				return;
			}
			info.nextRefr = info.thisRefr;
		} else info.outDir = zero_vec3;
	}

	double pdf(BSDFSampleInfo & info) const override {
		return k_d * glm::dot(info.outDir, info.normal) * INV_PI +
			k_s * (shininess + 1) * INV_PI * 0.5 * pow(std::max(0.0, glm::dot(info.outDir, glm::reflect(-info.inDir, info.normal))), shininess);
	}

	glm::dvec3 eval(BSDFSampleInfo & info) const override {
		return diffuse * INV_PI +
			one_vec3 * specular * (shininess + 2) * INV_PI * 0.5 *
			pow(std::max(0.0, glm::dot(info.outDir, glm::reflect(-info.inDir, info.normal))), shininess);
	}
};*/