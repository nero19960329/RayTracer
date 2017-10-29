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

void LambertianBRDF::brdfSample(BRDFSampleInfo & info) const {
	double u = info.rng.randomDouble();
	double k_d = std::max(info.surface->diffuse[0], std::max(info.surface->diffuse[1], info.surface->diffuse[2]));

	if (u < k_d) {
		double theta = std::acos(sqrt(info.rng.randomDouble()));
		double phi = 2.0 * PI * info.rng.randomDouble();
		info.outDir = change2World(info.normal, theta, phi);
		info.nextRefr = info.thisRefr;
	} else info.outDir = zero_vec3;
}

double LambertianBRDF::pdf(BRDFSampleInfo & info) const {
	double k_d = std::max(info.surface->diffuse[0], std::max(info.surface->diffuse[1], info.surface->diffuse[2]));
	return k_d * glm::dot(info.outDir, info.normal) * INV_PI;
}

glm::dvec3 LambertianBRDF::eval(BRDFSampleInfo & info) const {
	return info.surface->diffuse * INV_PI;
}

void PhongBRDF::brdfSample(BRDFSampleInfo & info) const {
	double u = info.rng.randomDouble();
	double k_d = std::max(info.surface->diffuse[0], std::max(info.surface->diffuse[1], info.surface->diffuse[2]));
	double k_s = info.surface->specular;
	double k_r = info.surface->refr;

	if (u < k_d) {
		double theta = std::asin(std::sqrt(1.0 - info.rng.randomDouble()));
		double phi = 2.0 * PI * info.rng.randomDouble();
		info.outDir = change2World(info.normal, theta, phi);
		info.nextRefr = info.thisRefr;
	} else if (u < k_d + k_s) {
		double theta = std::acos(std::pow(1.0 - info.rng.randomDouble(), 1.0 / (info.surface->shininess + 1)));
		double phi = 2.0 * PI * info.rng.randomDouble();
		glm::dvec3 ref = glm::reflect(-info.inDir, info.normal);
		info.outDir = change2World(ref, theta, phi);
		info.nextRefr = info.thisRefr;
	} else if (u < k_d + k_s + k_r) {
		double refrRatio = info.thisRefr / info.nextRefr;

		double ddn = glm::dot(-info.inDir, info.normal);
		double R0 = (1.0 - refrRatio) * (1.0 - refrRatio) / ((1.0 + refrRatio) * (1.0 + refrRatio));
		double Re = R0 + (1 - R0) * pow(1 + ddn, 5.0);
		double P = .25 + .5 * Re;

		glm::dvec3 reflDir = glm::reflect(-info.inDir, info.normal);
		glm::dvec3 refrDir = glm::refract(-info.inDir, info.normal, refrRatio);
		if (refrDir == zero_vec3) {
			info.outDir = reflDir;
			info.nextRefr = info.thisRefr;
		} else if (info.rng.randomDouble() < P) {
			info.outDir = reflDir;
			info.nextRefr = info.thisRefr;
		} else
			info.outDir = refrDir;
	} else info.outDir = zero_vec3;
}

double PhongBRDF::pdf(BRDFSampleInfo & info) const {
	double k_d = std::max(info.surface->diffuse[0], std::max(info.surface->diffuse[1], info.surface->diffuse[2]));
	double k_s = info.surface->specular;
	double k_r = info.surface->refr;

	if (info.nextRefr == info.thisRefr) {
		return k_d * glm::dot(info.outDir, info.normal) * INV_PI +
			k_s * (info.surface->shininess + 1) * INV_PI * 0.5 * pow(std::max(0.0, glm::dot(info.outDir, glm::reflect(-info.inDir, info.normal))), info.surface->shininess);
	} else {
		double ddn = glm::dot(-info.inDir, info.normal);
		double refrRatio = info.thisRefr / info.nextRefr;
		double R0 = (1.0 - refrRatio) * (1.0 - refrRatio) / ((1.0 + refrRatio) * (1.0 + refrRatio));
		double c = 1 + ddn;
		double Re = R0 + (1 - R0) * pow(c, 5.0);
		double Tr = 1.0 - Re;
		double P = .25 + .5 * Re;

		glm::dvec3 reflDir = glm::reflect(-info.inDir, info.normal);
		glm::dvec3 refrDir = glm::refract(-info.inDir, info.normal, refrRatio);
		if (info.outDir == refrDir)
			return k_r * glm::dot(info.outDir, info.normal) * (1 - P) / Tr;
		else if (info.outDir == reflDir && refrDir == zero_vec3)
			return k_r * glm::dot(info.outDir, info.normal);
		else if (info.outDir == reflDir && refrDir != zero_vec3)
			return k_r * glm::dot(info.outDir, info.normal) * P / Re;
		else
			return 0.0;
	}
}

glm::dvec3 PhongBRDF::eval(BRDFSampleInfo & info) const {
	if (info.nextRefr != info.thisRefr) {
		if (info.outDir == glm::refract(-info.inDir, info.normal, info.thisRefr / info.nextRefr))
			return info.surface->refr * one_vec3;
		else return zero_vec3;
	}

	return info.surface->diffuse * INV_PI +
			one_vec3 * info.surface->specular * (info.surface->shininess + 2) * INV_PI * 0.5 *
			pow(std::max(0.0, glm::dot(info.outDir, glm::reflect(-info.inDir, info.normal))), info.surface->shininess);
}
