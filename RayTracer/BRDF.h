#pragma once

#include "Surface.h"

#include <memory>

#include <iostream>

using namespace std;

enum BRDFType { LAMBERTIAN, PHONG };

class BRDF {
public:
	BRDF() {}
	virtual ~BRDF() {}

	virtual void brdfSample(const Vec3 &inDir, Vec3 &outDir, const Vec3 &normal) const = 0;
	virtual real_t pdf(const Vec3 &inDir, const Vec3 &outDir, const Vec3 &normal) const = 0;
	virtual Vec3 eval(const Vec3 &inDir, const Vec3 &outDir, const Vec3 &normal) const = 0;

	Vec3 change2World(const Vec3 &normal, real_t theta, real_t phi) const {
		real_t xs = sin(theta) * cos(phi);
		real_t ys = cos(theta);
		real_t zs = sin(theta) * sin(phi);

		Vec3 y = normal;
		Vec3 h = normal;
		if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z)) h.x = 1.0;
		else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z)) h.y = 1.0;
		else h.z = 1.0;

		Vec3 x = h.cross(y).getNormalized();
		Vec3 z = x.cross(y).getNormalized();

		return (xs * x + ys * y + zs * z).getNormalized();
	}
};

class LambertianBRDF : public BRDF {
private:
	const Vec3 &diffuse;

public:
	LambertianBRDF(const std::shared_ptr<Surface> &_surface) :
		diffuse(_surface->diffuse) {}

	void brdfSample(const Vec3 &, Vec3 &outDir, const Vec3 &normal) const override {
		real_t theta = acos(sqrt(1.0 - erand48()));
		real_t phi = 2.0 * PI * erand48();
		outDir = change2World(normal, theta, phi);
	}

	real_t pdf(const Vec3 &inDir, const Vec3 &outDir, const Vec3 &normal) const override { return std::max(0.0, outDir.dot(normal) * INV_PI); }

	Vec3 eval(const Vec3 &inDir, const Vec3 &outDir, const Vec3 &normal) const override {
		if (inDir.dot(normal) >= 0 && outDir.dot(normal) >= 0) return diffuse * INV_PI;
		else return Vec3::NONE;
	}
};

class PhongBRDF : public BRDF {
private:
	const std::shared_ptr<Surface> &surface;
	
	mutable bool diff_spec = false;

public:
	PhongBRDF(const std::shared_ptr<Surface> &_surface) :
		surface(_surface) {}
	
	void brdfSample(const Vec3 &inDir, Vec3 &outDir, const Vec3 &normal) const override {
		real_t u = erand48();
		real_t k_d = std::max(surface->diffuse[0], std::max(surface->diffuse[1], surface->diffuse[2]));
		real_t k_s = surface->specular;

		if (u < k_d) {
			diff_spec = false;
			real_t theta = acos(sqrt(1.0 - erand48()));
			real_t phi = 2.0 * PI * erand48();
			outDir = change2World(normal, theta, phi);
		} else if (u < k_d + k_s) {
			diff_spec = true;
			real_t theta = acos(pow(1.0 - erand48(), 1.0 / (surface->shininess + 1)));
			real_t phi = 2.0 * PI * erand48();
			outDir = change2World(normal, theta, phi);
		} else outDir = Vec3::zeros();
	}

	real_t pdf(const Vec3 &inDir, const Vec3 &outDir, const Vec3 &normal) const override {
		if (!diff_spec) return std::max(0.0, outDir.dot(normal) * INV_PI);
		else return (surface->shininess + 1) * INV_PI * 0.5 * pow(std::max(0.0, outDir.dot(inDir.reflection(normal))), surface->shininess);
	}

	Vec3 eval(const Vec3 &inDir, const Vec3 &outDir, const Vec3 &normal) const override {
		if (!diff_spec) return surface->diffuse * INV_PI;
		else return Vec3::ones() * surface->specular * (surface->shininess + 2) * 0.5 * pow(std::max(0.0, outDir.dot(inDir.reflection(normal))), surface->shininess) * INV_PI;
	}
};