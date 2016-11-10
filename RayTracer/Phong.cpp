#include "Constants.h"
#include "Scene.h"

using namespace std;

Vec3 Phong::color(const Ray &ray) const {
	DistRay distRay(ray, 0.0);
	return getColor(distRay);
}

Vec3 Phong::getColor(DistRay &ray, int depth) const {
	if (depth > maxDepth) {
		return Vec3::BLACK;
	}

	auto intersect = scene.getIntersect(ray);
	if (intersect) {
		IntersectInfo info = intersect->getIntersectInfo();

		Vec3 ambient = AMBIENT_INTENSITY * info.surface->ambient;
		Vec3 local = getPhongLocal(info, ray);
		ray.dist += intersect->getDistToInter();
		local *= exp(-ray.dist * AIR_BEER_DENSITY);

		Vec3 refl = getReflection(info, ray, depth);
		Vec3 refr = getRefraction(info, ray, depth);

		return ambient + local + refl + refr;
	} else {
		return Vec3::BLACK;
	}
}

Vec3 Phong::getPhongLocal(const IntersectInfo &info, const DistRay &ray, int depth) const {
	Vec3 diffuse = Vec3::zeros();
	Vec3 specular = Vec3::zeros();

	Vec3 N = info.normal;
	Vec3 V = (ray.orig - info.interPoint).getNormalized();
	for (auto &light : scene.lights) {
		if (isShadow(light, info)) {
			continue;
		}
	
		Vec3 toLight = light->getCenter() - info.interPoint;
		Vec3 L = toLight.getNormalized();
		Vec3 I = light->color * light->intensity * exp(-toLight.norm() * AIR_BEER_DENSITY);
		real_t tmp = L.dot(N);
		if (tmp > 0) diffuse += I.mul(info.surface->diffuse) * tmp;

		if (info.surface->specular > epsilon) {
			Vec3 R = L.reflection(N).getNormalized();
			tmp = R.dot(V);
			if (tmp > 0) specular += I * info.surface->specular * pow(tmp, info.surface->shininess);
		}
	}

	return diffuse + specular;
}

Vec3 Phong::getReflection(const IntersectInfo &info, const DistRay &ray, int depth) const {
	if (info.surface->refl < epsilon) {
		return Vec3::BLACK;
	}

	Vec3 N = info.normal;
	Vec3 V = (ray.orig - info.interPoint).getNormalized();
	Vec3 R = V.reflection(N).getNormalized();
	DistRay newRay(Ray{ info.interPoint + epsilon * R, R, ray.refrIdx }, ray.dist);
	Vec3 reflection = getColor(newRay, depth + 1);

	return info.surface->refl * reflection;
}

Vec3 Phong::getRefraction(const IntersectInfo &info, const DistRay &ray, int depth) const {
	if (info.surface->refr < epsilon) {
		return Vec3::BLACK;
	}

	Vec3 N = info.normal;
	Vec3 V = (ray.orig - info.interPoint).getNormalized();
	Vec3 T = V.refraction(N, info.nextRefrIdx / ray.refrIdx);
	DistRay newRay(Ray{ info.interPoint + epsilon * T, T, info.nextRefrIdx }, ray.dist);
	Vec3 refraction = getColor(newRay, depth + 1);

	return info.surface->refr * refraction;
}

bool Phong::isShadow(const shared_ptr<Light> &light, const IntersectInfo &info) const {
	Vec3 toLight = light->getCenter() - info.interPoint;
	Vec3 dirToLight = toLight.getNormalized();
	real_t distToLight = toLight.norm();
	Ray shadowTestRay{ info.interPoint + epsilon * dirToLight, dirToLight };
	return scene.isIntersect(shadowTestRay, distToLight);
}