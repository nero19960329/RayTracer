#include "brdf.h"

glm::dvec3 BRDF::change2World(const glm::dvec3 & normal, double theta, double phi) const {
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

void BRDF::uniformSample(BRDFSampleInfo & info) const {
	double theta = std::asin(std::sqrt(1.0 - info.rng.randomDouble()));
	double phi = 2.0 * PI * info.rng.randomDouble();
	info.outDir = change2World(info.normal, theta, phi);
}

double LambertianBRDF::brdfSample(BRDFSampleInfo & info) const {
	double theta = std::acos(sqrt(info.rng.randomDouble()));
	double phi = 2.0 * PI * info.rng.randomDouble();
	info.outDir = change2World(info.normal, theta, phi);
	return std::max(0.0, glm::dot(info.outDir, info.normal) * INV_PI);
}

glm::dvec3 LambertianBRDF::eval(BRDFSampleInfo & info) const {
	if (glm::dot(info.inDir, info.normal) >= 0 && 
		glm::dot(info.outDir, info.normal) >= 0) return info.surface->diffuse * INV_PI;
	else return zero_vec3;
}

double PhongBRDF::brdfSample(BRDFSampleInfo & info) const {
	double u = info.rng.randomDouble();
	double k_d = std::max(info.surface->diffuse[0], std::max(info.surface->diffuse[1], info.surface->diffuse[2]));
	double k_s = info.surface->specular;
	double k_r = info.surface->refr;

	if (u < k_d) {
		double theta = std::asin(std::sqrt(1.0 - info.rng.randomDouble()));
		double phi = 2.0 * PI * info.rng.randomDouble();
		info.outDir = change2World(info.normal, theta, phi);
		return k_d * std::max(0.0, glm::dot(info.outDir, info.normal) * INV_PI);
	} else if (u < k_d + k_s) {
		double theta = std::acos(std::pow(1.0 - info.rng.randomDouble(), 1.0 / (info.surface->shininess + 1)));
		double phi = 2.0 * PI * info.rng.randomDouble();
		glm::dvec3 ref = glm::reflect(-info.inDir, info.normal);
		info.outDir = change2World(ref, theta, phi);
		return k_s * (info.surface->shininess + 1) * INV_PI * 0.5 *
			pow(std::max(0.0, glm::dot(info.outDir, glm::reflect(-info.inDir, info.normal))), info.surface->shininess);
	} else if (u < k_d + k_s + k_r) {
		info.outDir = glm::refract(-info.inDir, info.normal, info.refrRatio);
		return -1.0;
	} else {
		info.outDir = zero_vec3;
		return 0.0;
	}
}

glm::dvec3 PhongBRDF::eval(BRDFSampleInfo & info) const {
	return info.surface->diffuse * INV_PI +
			one_vec3 * info.surface->specular * (info.surface->shininess + 2) * INV_PI * 0.5 *
			pow(std::max(0.0, glm::dot(info.outDir, glm::reflect(-info.inDir, info.normal))), info.surface->shininess);
}
