#pragma once

#include <glm.hpp>

#include "constants.h"

class Material {
public:
	double ambient, specular, shininess;
	double refl, refr;
	glm::dvec3 emission;

public:
	Material(double ambient_ = DEFAULT_AMBIENT, double specular_ = DEFAULT_SPECULAR, double shininess_ = DEFAULT_SHININESS, double refl_ = 0.0, double refr_ = 0.0, glm::dvec3 emission_ = zero_vec3) :
		ambient(ambient_), specular(specular_), shininess(shininess_), refl(refl_), refr(refr_), emission(emission_) {}
};

class Surface : public Material {
public:
	glm::dvec3 diffuse;

public:
	Surface(const Material &material, glm::dvec3 diffuse_) :
		Material(material), diffuse(diffuse_) {}
};