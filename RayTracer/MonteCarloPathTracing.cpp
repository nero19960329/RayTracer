#include "MonteCarloPathTracing.h"

pair<Vec3, Vec3> MonteCarloPathTracing::color(const Ray &ray, RNGenerator *rng) const {
	DistRay distRay(ray, 0.0);
	return getColor(distRay, rng);
}

pair<Vec3, Vec3> MonteCarloPathTracing::getColor(DistRay &ray, RNGenerator *rng, int depth) const {
	auto intersect = scene.getIntersect(ray);
	if (!intersect) return{ Vec3::NONE, Vec3::NONE };

	IntersectInfo info = intersect->getIntersectInfo();
	pair<Vec3, Vec3> radiance = getReflectedRadiance(ray, info, rng, depth);
	return{ info.surface->emission + radiance.first, radiance.second };
}

pair<Vec3, Vec3> MonteCarloPathTracing::getReflectedRadiance(DistRay &ray, const IntersectInfo &info, RNGenerator *rng, int depth) const {
	auto vertices = scene.lights[0]->vertices();
	return{ getDirectRadiance(ray, info, rng), getIndirectRadiance(ray, info, rng, depth) };
}

Vec3 MonteCarloPathTracing::getDirectRadiance(DistRay &ray, const IntersectInfo &info, RNGenerator *rng) const {
	Vec3 inDir = (ray.orig - info.interPoint).getNormalized();
	Vec3 outDir;
	if (scene.lights.size() == 1) {
		if (!info.surface->emission.isZero()) return Vec3::NONE;
		else scene.lights[0]->luminaireSample(*rng, info.interPoint, outDir);
	} else error_exit("Only support one light!\n");

	real_t cos_theta = outDir.dot(info.normal);
	if (cos_theta <= 0.0) return Vec3::NONE;

	DistRay newRay(Ray{ info.interPoint + outDir * epsilon, outDir }, ray.dist);
	auto intersect = scene.getIntersect(newRay);
	if (!intersect) return Vec3::NONE;

	IntersectInfo newInfo = intersect->getIntersectInfo();
	return brdf->eval(*rng, inDir, outDir, info.normal, info.surface, DIFFUSE).mul(newInfo.surface->emission) * cos_theta * std::max(0.0, (-outDir).dot(newInfo.normal)) /
		((info.interPoint - newInfo.interPoint).sqr() * (2.0 / scene.lights[0]->v1crossv2()));
}

Vec3 MonteCarloPathTracing::getIndirectRadiance(DistRay &ray, const IntersectInfo &info, RNGenerator *rng, int depth) const {
	real_t p = 1.0;
	if (depth >= minDepth) {
		p = RUSSIAN_ROULETTE_POSSIBILITY;
		if (rng->randomReal() >= p) return Vec3::NONE;
	}

	Vec3 inDir = (ray.orig - info.interPoint).getNormalized();
	Vec3 outDir;
	ReflRayType reflType = brdf->brdfSample(*rng, inDir, outDir, info.normal, info.surface);

	real_t cos_theta = outDir.dot(info.normal);
	if (cos_theta <= 0.0) return Vec3::NONE;

	DistRay newRay(Ray{ info.interPoint + outDir * epsilon, outDir }, ray.dist);

	auto intersect = scene.getIntersect(newRay);
	if (!intersect) return Vec3::NONE;

	IntersectInfo newInfo = intersect->getIntersectInfo();

	real_t pdf = brdf->pdf(*rng, inDir, outDir, info.normal, info.surface, reflType);

	pair<Vec3, Vec3> radiance = getReflectedRadiance(newRay, newInfo, rng, depth + 1);
	if (pdf > 0.0) return brdf->eval(*rng, inDir, outDir, info.normal, info.surface, reflType).mul(radiance.first + radiance.second) * cos_theta / (p * pdf);

	return Vec3::NONE;
}