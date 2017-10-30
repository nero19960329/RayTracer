#include "ray_tracing.h"

glm::dvec3 RayTracing::color(const Ray & ray, RNG * rng) const {
	DistRay distRay(ray, 0.0);
	return getColor(distRay);
}

glm::dvec3 RayTracing::getColor(DistRay & ray, int depth) const {
	if (depth > maxDepth) return zero_vec3;

	auto intersect = scene.getIntersect(ray);
	if (intersect) {
		IntersectInfo info = intersect->getIntersectInfo();

		glm::dvec3 emission = info.material->emission;
		glm::dvec3 ambient = AMBIENT_INTENSITY * info.material->bsdf->toRayTracingBSDF()->ambient;
		glm::dvec3 local = getPhongLocal(info, ray);
		ray.dist += intersect->getDistToInter();
		local *= std::exp(-ray.dist * DEFAULT_ATTENUATION_COEFFICIENT);

		glm::dvec3 refl = getReflection(info, ray, depth);
		glm::dvec3 refr = getRefraction(info, ray, depth);

		return emission + ambient + local + refl + refr;
	} else return zero_vec3;
}

glm::dvec3 RayTracing::getPhongLocal(const IntersectInfo & info, const DistRay & ray, int depth) const {
	glm::dvec3 diffuse = zero_vec3;
	glm::dvec3 specular = zero_vec3;

	glm::dvec3 N = info.normal;
	glm::dvec3 V = glm::normalize(ray.ori - info.interPoint);
	for (auto & light : scene.lights) {
		if (isShadow(light, info)) continue;

		glm::dvec3 toLight = light->center - info.interPoint;
		glm::dvec3 L = glm::normalize(toLight);
		glm::dvec3 I = light->color * light->intensity * std::exp(-glm::length(toLight) * DEFAULT_ATTENUATION_COEFFICIENT);
		double tmp = glm::dot(L, N);
		if (tmp > 0.0) diffuse += I * info.material->bsdf->toRayTracingBSDF()->diffuse * tmp;

		if (info.material->bsdf->toRayTracingBSDF()->specular > eps) {
			glm::dvec3 R = glm::normalize(glm::reflect(-L, N));
			tmp = glm::dot(R, V);
			if (tmp > 0.0) specular += I * info.material->bsdf->toRayTracingBSDF()->specular * std::pow(tmp, info.material->bsdf->toRayTracingBSDF()->shininess);
		}
	}
	
	return diffuse + specular;
}

glm::dvec3 RayTracing::getReflection(const IntersectInfo & info, const DistRay & ray, int depth) const {
	if (info.material->bsdf->toRayTracingBSDF()->refl < eps) return zero_vec3;

	glm::dvec3 N = info.normal;
	glm::dvec3 V = glm::normalize(ray.ori - info.interPoint);
	glm::dvec3 R = glm::normalize(glm::reflect(-V, N));
	DistRay newRay(Ray{ info.interPoint + eps * R, R, ray.refrIdx }, ray.dist);
	glm::dvec3 reflection = getColor(newRay, depth + 1);

	return info.material->bsdf->toRayTracingBSDF()->refl * reflection;
}

glm::dvec3 RayTracing::getRefraction(const IntersectInfo & info, const DistRay & ray, int depth) const {
	if (info.material->bsdf->toRayTracingBSDF()->refr < eps) return zero_vec3;

	glm::dvec3 N = info.normal;
	glm::dvec3 V = glm::normalize(ray.ori - info.interPoint);
	glm::dvec3 T = glm::refract(-V, N, ray.refrIdx / info.nextRefrIdx);
	DistRay newRay(Ray{ info.interPoint + eps * T, T, info.nextRefrIdx }, ray.dist);
	glm::dvec3 refraction = getColor(newRay, depth + 1);

	return info.material->bsdf->toRayTracingBSDF()->refr * refraction;
}

bool RayTracing::isShadow(Light * light, const IntersectInfo & info) const {
	glm::dvec3 toLight = light->center - info.interPoint;
	glm::dvec3 dirToLight = glm::normalize(toLight);
	double distToLight = glm::length(toLight);
	Ray shadowTestRay{ info.interPoint + eps * dirToLight, dirToLight };
	return scene.isIntersect(shadowTestRay, distToLight);
}
