#include "Constants.h"
#include "DataGenerator.h"
#include "PredictRayTracing.h"

using namespace std;

pair<Vec3, Vec3> PredictRayTracing::color(const Ray &ray, RNGenerator *) const {
	DistRay distRay(ray, 0.0);
	return{ getColor(distRay), Vec3::NONE };
}

Vec3 PredictRayTracing::getColor(DistRay &ray, int depth) const {
	auto intersect = scene.getIntersect(ray);
	if (intersect) {
		IntersectInfo info = intersect->getIntersectInfo();

		Vec3 emission = info.surface->emission;
		if (!emission.isZero()) return emission;

		const NeuralNetwork &direct = directNNVec[info.objNum];
		const NeuralNetwork &indirect = indirectNNVec[info.objNum];
		DataGenerator::TrainData input{ info.interPoint, ray.dir, Vec3::zeros(), info.normal, info.surface->diffuse };
		return direct.predict(input) + indirect.predict(input);
	} else return Vec3::NONE;
}