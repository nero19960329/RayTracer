#pragma once

#include <glm.hpp>

#include <memory>

//#include "bsdf.h"
#include "constants.h"

class BSDF;

class Material {
public:
	std::shared_ptr<BSDF> bsdf;
	glm::dvec3 emission;

public:
	Material(const std::shared_ptr<BSDF> bsdf_ = nullptr, glm::dvec3 emission_ = zero_vec3) : bsdf(bsdf_), emission(emission_) {}

	static std::shared_ptr<Material> clone(const std::shared_ptr<Material> & material);
};