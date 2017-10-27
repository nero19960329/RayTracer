#pragma once

#include "material.h"
#include "rng.h"

#include <memory>

#include <glm.hpp>

enum BRDFType { LAMBERTIAN, PHONG };

struct BRDFSampleInfo {
	RNG & rng;
	glm::dvec3 & inDir;
	glm::dvec3 & outDir;
	const glm::dvec3 & normal;
	const std::shared_ptr<Surface> & surface;
	double thisRefr, nextRefr;

	BRDFSampleInfo(RNG & rng_, glm::dvec3 & inDir_, glm::dvec3 & outDir_, const glm::dvec3 & normal_, const std::shared_ptr<Surface> & surface_, double thisRefr_, double nextRefr_) :
		rng(rng_), inDir(inDir_), outDir(outDir_), normal(normal_), surface(surface_), thisRefr(thisRefr_), nextRefr(nextRefr_) {}
};

class BRDF {
public:
	BRDF() {}

	virtual void brdfSample(BRDFSampleInfo & info) const = 0;
	virtual double pdf(BRDFSampleInfo & info) const = 0;
	virtual glm::dvec3 eval(BRDFSampleInfo & info) const = 0;

	glm::dvec3 change2World(const glm::dvec3 &normal, double theta, double phi) const;

	void uniformSample(BRDFSampleInfo & info) const;
};

class LambertianBRDF : public BRDF {
public:
	LambertianBRDF() {}

	void brdfSample(BRDFSampleInfo & info) const override;
	double pdf(BRDFSampleInfo & info) const override;
	glm::dvec3 eval(BRDFSampleInfo & info) const override;
};

class PhongBRDF : public BRDF {
public:
	PhongBRDF() {}

	void brdfSample(BRDFSampleInfo & info) const override;
	double pdf(BRDFSampleInfo & info) const override;
	glm::dvec3 eval(BRDFSampleInfo & info) const override;
};