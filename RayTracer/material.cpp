#include "bsdf.h"
#include "material.h"

std::shared_ptr<Material> Material::clone(const std::shared_ptr<Material> & material) {
	std::shared_ptr<Material> res = std::make_shared<Material>();
	res->bsdf = material->bsdf->clone();
	res->emission = material->emission;
	return res;
}