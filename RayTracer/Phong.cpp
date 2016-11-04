#include "Constants.h"
#include "Scene.h"

using namespace std;

Vec3 Phong::getColor(const Ray &ray) const {
	auto intersect = scene.getIntersect(ray);
	if (intersect) {
		IntersectInfo info = scene.getIntersect(ray)->getIntersectInfo();

		Vec3 ambient = AMBIENT_INTENSITY * info.surface->ambient;
		Vec3 local = getPhongLocal(info, ray);

		return ambient + local;
	} else {
		return Vec3::BLACK;
	}
}

Vec3 Phong::getPhongLocal(const IntersectInfo &info, const Ray &ray) const {
	Vec3 diffuse = Vec3::zeros();
	Vec3 specular = Vec3::zeros();

	Vec3 N = info.normal;
	Vec3 V = (ray.orig - info.interPoint).getNormalized();
	for (auto &light : scene.lights) {
		if (isShadow(light, info)) {
			continue;
		}

		Vec3 L = (light->getCenter() - info.interPoint).getNormalized();
		Vec3 I = light->getIntensity(0);
		real_t tmp = L.dot(N);
		if (tmp > 0) {
			diffuse += I.mul(info.surface->diffuse) * L.dot(N);
		}

		Vec3 R = (2 * N * N.dot(L) - L).getNormalized();
		tmp = R.dot(V);
		if (tmp > 0) {
			specular += I * info.surface->specular * pow(R.dot(V), info.surface->shininess);
		}
	}

	return diffuse + specular;
}

bool Phong::isShadow(const shared_ptr<Light> &light, const IntersectInfo &info) const {
	Vec3 toLight = light->getCenter() - info.interPoint;
	Vec3 dirToLight = toLight.getNormalized();
	real_t distToLight = toLight.norm();
	Ray shadowTestRay{ info.interPoint + epsilon * dirToLight, dirToLight };
	return scene.isIntersect(shadowTestRay, distToLight);
}