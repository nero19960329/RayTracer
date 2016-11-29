#include "MonteCarloPathTracing.h"

real_t getTriAngle(const Vec3 &O, const Vec3 &A, const Vec3 &B) {
	real_t o = (A - B).norm(), a = (O - B).norm(), b = (O - A).norm();
	return acos((sqr(a) + sqr(b) - sqr(o)) / (2 * a * b));
}

real_t getSolidAngle(const Vec3 &O, const Vec3 &A, const Vec3 &B, const Vec3 &C) {
	real_t alpha, beta, gamma;
	alpha = getTriAngle(O, B, C);
	beta = getTriAngle(O, A, C);
	gamma = getTriAngle(O, A, B);
	real_t s = (alpha + beta + gamma) * 0.5;
	real_t omega = atan(sqrt(tan(s * 0.5) * tan((s - alpha) * 0.5) * tan((s - beta) * 0.5) * tan((s - gamma) * 0.5)));
	if (omega < 0) return 4 * (omega + PI);
	else return 4 * omega;
}

real_t getSolidAngle(const Vec3 &O, const Vec3 &A, const Vec3 &B, const Vec3 &C, const Vec3 &D) {
	return getSolidAngle(O, A, B, C) + getSolidAngle(O, C, D, A);
}

pair<Vec3, Vec3> MonteCarloPathTracing::color(const Ray &ray) const {
	DistRay distRay(ray, 0.0);
	return getColor(distRay);
}

pair<Vec3, Vec3> MonteCarloPathTracing::getColor(DistRay &ray, int depth) const {
	auto intersect = scene.getIntersect(ray);
	if (!intersect) return{ Vec3::NONE, Vec3::NONE };

	IntersectInfo info = intersect->getIntersectInfo();
	pair<Vec3, Vec3> radiance = getReflectedRadiance(ray, info, depth);
	return{ info.surface->emission + radiance.first, radiance.second };
}

pair<Vec3, Vec3> MonteCarloPathTracing::getReflectedRadiance(DistRay &ray, const IntersectInfo &info, int depth) const {
	auto vertices = lights[0]->vertices();
	return{ getDirectRadiance(ray, info), getIndirectRadiance(ray, info, depth) };
}

Vec3 MonteCarloPathTracing::getDirectRadiance(DistRay &ray, const IntersectInfo &info) const {
	Vec3 inDir = (ray.orig - info.interPoint).getNormalized();
	Vec3 outDir;
	if (lights.size() == 1) {
		if (!info.surface->emission.isZero()) return Vec3::NONE;
		else lights[0]->luminaireSample(info.interPoint, outDir);
	} else error_exit("Only support one light!\n");

	real_t cos_theta = outDir.dot(info.normal);
	if (cos_theta <= 0.0) return Vec3::NONE;

	DistRay newRay(Ray{ info.interPoint + outDir * epsilon, outDir }, ray.dist);
	auto intersect = scene.getIntersect(newRay);
	if (!intersect) return Vec3::NONE;

	IntersectInfo newInfo = intersect->getIntersectInfo();
	return brdf->eval(inDir, outDir, info.normal, info.surface, DIFFUSE).mul(newInfo.surface->emission) * cos_theta * std::max(0.0, (-outDir).dot(newInfo.normal)) /
		((info.interPoint - newInfo.interPoint).sqr() * (2.0 / lights[0]->v1crossv2()));
}

Vec3 MonteCarloPathTracing::getIndirectRadiance(DistRay &ray, const IntersectInfo &info, int depth) const {
	real_t p = 1.0;
	if (depth >= minDepth) {
		p = RUSSIAN_ROULETTE_POSSIBILITY;
		if (erand48() >= p) return Vec3::NONE;
	}

	Vec3 inDir = (ray.orig - info.interPoint).getNormalized();
	Vec3 outDir;
	ReflRayType reflType = brdf->brdfSample(inDir, outDir, info.normal, info.surface);

	real_t cos_theta = outDir.dot(info.normal);
	if (cos_theta <= 0.0) return Vec3::NONE;

	DistRay newRay(Ray{ info.interPoint + outDir * epsilon, outDir }, ray.dist);

	auto intersect = scene.getIntersect(newRay);
	if (!intersect) return Vec3::NONE;

	IntersectInfo newInfo = intersect->getIntersectInfo();

	real_t pdf = brdf->pdf(inDir, outDir, info.normal, info.surface, reflType);

	pair<Vec3, Vec3> radiance = getReflectedRadiance(newRay, newInfo, depth + 1);
	if (pdf > epsilon) return brdf->eval(inDir, outDir, info.normal, info.surface, reflType).mul(radiance.first + radiance.second) * cos_theta / (p * pdf);

	return Vec3::NONE;
}