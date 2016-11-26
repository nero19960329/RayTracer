#include "BRDF.h"
#include "MonteCarloPathTracing.h"

Vec3 MonteCarloPathTracing::color(const Ray &ray) const {
	DistRay distRay(ray, 0.0);
	Vec3 color = getColor(distRay);
	return color;
}

Vec3 MonteCarloPathTracing::getColor(DistRay &ray, int depth) const {
	auto intersect = scene.getIntersect(ray);
	if (!intersect) return Vec3::NONE;

	IntersectInfo info = intersect->getIntersectInfo();
	Vec3 res = info.surface->emission;

	real_t p = 1.0;
	if (depth > minDepth) {
		p = RUSSIAN_ROULETTE_POSSIBILITY;
		if (erand48() >= p) return res;
	}

	Vec3 inDir = (ray.orig - info.interPoint).getNormalized();
	Vec3 outDir;
	//LambertianBRDF brdf(info.surface);
	PhongBRDF brdf(info.surface);
	brdf.brdfSample(inDir, outDir, info.normal);

	DistRay newRay(Ray{ info.interPoint + outDir * epsilon, outDir }, ray.dist);

	real_t cos_alpha = std::max(0.0, outDir.dot(info.normal));
	real_t pdf = brdf.pdf(inDir, outDir, info.normal);
	if (pdf > epsilon) res += brdf.eval(inDir, outDir, info.normal).mul(getColor(newRay, depth + 1)) * cos_alpha / (p * pdf);
	return res;
}