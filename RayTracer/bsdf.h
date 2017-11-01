#pragma once

#include "constants.h"
#include "object.h"
#include "ray.h"
#include "rng.h"

#include <memory>

#include <glm.hpp>

/*struct BSDFSampleInfo {
	RNG & rng;
	glm::dvec3 & inDir;
	glm::dvec3 & outDir;
	const glm::dvec3 & normal;
	double thisRefr, nextRefr;

	double fresnelTerm;

	BSDFSampleInfo(RNG & rng_, glm::dvec3 & inDir_, glm::dvec3 & outDir_, const glm::dvec3 & normal_, double thisRefr_, double nextRefr_) :
		rng(rng_), inDir(inDir_), outDir(outDir_), normal(normal_), thisRefr(thisRefr_), nextRefr(nextRefr_) {}
};*/

class RayTracingBSDF;
class BSDFSampler;

class BSDF {
public:
	BSDF() {}

	virtual void setColor(glm::dvec3 color) {}
	virtual std::shared_ptr<BSDFSampler> getSampler(const Ray & ray, const IntersectInfo & info) const { return nullptr; };
	virtual RayTracingBSDF * toRayTracingBSDF() { return nullptr; }
	virtual std::shared_ptr<BSDF> clone() const = 0;
};

class BSDFSampler {
protected:
	const Ray & ray;
	const IntersectInfo & info;
	mutable glm::dvec3 outDir;

public:
	BSDFSampler(const Ray & ray_, const IntersectInfo & info_) :
		ray(ray_), info(info_), outDir(zero_vec3) {}

	virtual void setOutDir(const glm::dvec3 & outDir_) const { outDir = outDir_; }

	virtual Ray sample(RNG * rng) const = 0;
	virtual double pdf() const = 0;
	virtual glm::dvec3 eval() const = 0;

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

	std::shared_ptr<BSDF> clone() const override { return std::make_shared<RayTracingBSDF>(*this); }
};