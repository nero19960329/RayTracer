#pragma once

#include "constants.h"
#include "rng.h"

#include <memory>

#include <glm.hpp>

struct BSDFSampleInfo {
	RNG & rng;
	glm::dvec3 & inDir;
	glm::dvec3 & outDir;
	const glm::dvec3 & normal;
	double thisRefr, nextRefr;

	BSDFSampleInfo(RNG & rng_, glm::dvec3 & inDir_, glm::dvec3 & outDir_, const glm::dvec3 & normal_, double thisRefr_, double nextRefr_) :
		rng(rng_), inDir(inDir_), outDir(outDir_), normal(normal_), thisRefr(thisRefr_), nextRefr(nextRefr_) {}
};

class RayTracingBSDF;

class BSDF {
public:
	BSDF() {}

	virtual void setColor(glm::dvec3 color) {}

	virtual void bsdfSample(BSDFSampleInfo & info) const {};
	virtual double pdf(BSDFSampleInfo & info) const { return 0.0; };
	virtual glm::dvec3 eval(BSDFSampleInfo & info) const { return zero_vec3; };

	glm::dvec3 change2World(const glm::dvec3 &normal, double theta, double phi) const {
		double xs = std::sin(theta) * std::cos(phi);
		double ys = std::cos(theta);
		double zs = std::sin(theta) * std::sin(phi);

		glm::dvec3 y = normal;
		glm::dvec3 h = normal;
		if (std::abs(h.x) <= std::abs(h.y) && std::abs(h.x) <= std::abs(h.z)) h.x = 1.0;
		else if (std::abs(h.y) <= std::abs(h.x) && std::abs(h.y) <= std::abs(h.z)) h.y = 1.0;
		else h.z = 1.0;

		glm::dvec3 x = glm::normalize(glm::cross(h, y));
		glm::dvec3 z = glm::normalize(glm::cross(x, y));

		return glm::normalize(xs * x + ys * y + zs * z);
	}

	virtual RayTracingBSDF * toRayTracingBSDF() { return (RayTracingBSDF *) this; }
};

class RayTracingBSDF : public BSDF {
public:
	double ambient, specular, shininess;
	glm::dvec3 diffuse;
	double refl, refr;

public:
	RayTracingBSDF(double ambient_ = DEFAULT_AMBIENT, double specular_ = DEFAULT_SPECULAR, double shininess_ = DEFAULT_SHININESS, double refl_ = 0.0, double refr_ = 0.0) :
		ambient(ambient_), specular(specular_), shininess(shininess_), refl(refl_), refr(refr_) {}

	void setColor(glm::dvec3 color) override { diffuse = color; }
	RayTracingBSDF * toRayTracingBSDF() { return this; }
};