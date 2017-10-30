#pragma once

#include "bsdf.h"

class IdealRefrBSDF : public BSDF {
public:
	IdealRefrBSDF() {}

	void bsdfSample(BSDFSampleInfo & info) const override {
		double refrRatio = info.thisRefr / info.nextRefr;

		double ddn = glm::dot(-info.inDir, info.normal);
		double R0 = (1.0 - refrRatio) * (1.0 - refrRatio) / ((1.0 + refrRatio) * (1.0 + refrRatio));
		double Re = R0 + (1 - R0) * pow(1 + ddn, 5.0);

		double u = info.rng.randomDouble();

		glm::dvec3 reflDir = glm::reflect(-info.inDir, info.normal);
		glm::dvec3 refrDir = glm::refract(-info.inDir, info.normal, refrRatio);
		if (refrDir == zero_vec3) {
			info.outDir = reflDir;
			info.nextRefr = info.thisRefr;
		} else if (u < Re) {
			info.outDir = reflDir;
			info.nextRefr = info.thisRefr;
		} else
			info.outDir = refrDir;
	}

	double pdf(BSDFSampleInfo & info) const override {
		double p = 1.0 / std::abs(glm::dot(info.outDir, info.normal));

		double ddn = glm::dot(-info.inDir, info.normal);
		double refrRatio = info.thisRefr / info.nextRefr;
		double R0 = (1.0 - refrRatio) * (1.0 - refrRatio) / ((1.0 + refrRatio) * (1.0 + refrRatio));
		double c = 1 + ddn;
		double Re = R0 + (1 - R0) * pow(c, 5.0);

		glm::dvec3 reflDir = glm::reflect(-info.inDir, info.normal);
		glm::dvec3 refrDir = glm::refract(-info.inDir, info.normal, refrRatio);

		if (info.outDir == reflDir && refrDir == zero_vec3) {	// 全反射
			return p;
		} else if (info.outDir == refrDir) {	// 折射
			return (1 - Re) * p;
		} else if (info.outDir == reflDir && refrDir != zero_vec3) {	// 反射
			return Re * p;
		} else return 0.0;
	}

	glm::dvec3 eval(BSDFSampleInfo & info) const override {
		double p = 1.0 / std::abs(glm::dot(info.outDir, info.normal));

		double ddn = glm::dot(-info.inDir, info.normal);
		double refrRatio = info.thisRefr / info.nextRefr;
		double R0 = (1.0 - refrRatio) * (1.0 - refrRatio) / ((1.0 + refrRatio) * (1.0 + refrRatio));
		double c = 1 + ddn;
		double Re = R0 + (1 - R0) * pow(c, 5.0);

		glm::dvec3 reflDir = glm::reflect(-info.inDir, info.normal);
		glm::dvec3 refrDir = glm::refract(-info.inDir, info.normal, refrRatio);

		if (info.outDir == reflDir && refrDir == zero_vec3) {	// 全反射
			return one_vec3 * p;
		} else if (info.outDir == refrDir) {	// 折射
			return (1 - Re) * p * one_vec3 / (refrRatio * refrRatio);
		} else if (info.outDir == reflDir && refrDir != zero_vec3) {	// 反射
			return Re * p * one_vec3;
		} else return zero_vec3;
	}
};