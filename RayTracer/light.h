#pragma once

#include <glm.hpp>

#include "face.h"
#include "object.h"
#include "rng.h"

class Light : public Object {
public:
	glm::dvec3 center;
	glm::dvec3 color;
	double intensity;
	double area;

public:
	Light() : Object(nullptr) {}
	Light(glm::dvec3 center_, glm::dvec3 color_, double intensity_) : 
		Object(new PureTexture(Material(0.0, 0.0, 0.0, 0.0, 0.0, color_ * intensity_), color_)), center(center_), color(color_), intensity(intensity_) {}

	virtual void luminaireSample(RNG & rng, glm::dvec3 interPoint, glm::dvec3 & outDir) const = 0;
	virtual double pdf(RNG & rng, glm::dvec3 interPoint, glm::dvec3 & outDir) const = 0;
	virtual bool equals(const Object * obj) const { return obj == this; };
};

class PointLight : public Light {
public:
	PointLight(glm::dvec3 center_, glm::dvec3 color_, double intensity_) : Light(center_, color_, intensity_) { area = 0.0; }

	void luminaireSample(RNG & rng, glm::dvec3 interPoint, glm::dvec3 & outDir) const override;
	double pdf(RNG & rng, glm::dvec3 interPoint, glm::dvec3 & outDir) const override;
	std::shared_ptr<Intersect> getTrace(const Ray & ray, double dist = std::numeric_limits<double>::max()) const override;
};

class RectLight : public Light {
private:
	glm::dvec3 x, y, n;
	Face f1, f2;

public:
	RectLight(glm::dvec3 center_, glm::dvec3 x_, glm::dvec3 y_, glm::dvec3 n_, glm::dvec3 color_, double intensity_) : Light(center_, color_, intensity_), x(x_), y(y_), n(n_) {
		glm::dvec3 LT, RT, LB, RB;
		LT = center_ + (-x + y) * 0.5;
		RT = center_ + (x + y) * 0.5;
		LB = center_ + (-x - y) * 0.5;
		RB = center_ + (x - y) * 0.5;
		if (glm::dot(n, glm::cross(RT - LT, LB - LT)) > 0.0) {
			f1 = Face(texture, LT, RT, LB);
			f2 = Face(texture, RT, RB, LB);
		} else {
			f1 = Face(texture, LT, LB, RT);
			f2 = Face(texture, RT, LB, RB);
		}
		area = glm::length(x) * glm::length(y);
	}

	void luminaireSample(RNG & rng, glm::dvec3 interPoint, glm::dvec3 & outDir) const override;
	double pdf(RNG & rng, glm::dvec3 interPoint, glm::dvec3 & outDir) const override;
	std::shared_ptr<Intersect> getTrace(const Ray & ray, double dist = std::numeric_limits<double>::max()) const override;
	bool equals(const Object * obj) const override { return obj == &f1 || obj == &f2; }
};