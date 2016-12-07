#include "DataGenerator.h"

#include <ctime>
#include <fstream>
#include <vector>

using namespace std;

pair<DataGenerator::TrainData, DataGenerator::TrainData> DataGenerator::getSingleTrainData(RNGenerator *rng, const Vec3 &viewPoint, const Vec3 &rayDir, int &objNum) const {
	Ray ray{ viewPoint, rayDir };

	if (!scene.isIntersect(ray, numeric_limits<real_t>::max())) {
		objNum = -1;
		return{};
	}

	auto intersect = scene.getIntersect(ray);
	IntersectInfo info = intersect->getIntersectInfo();

	Vec3 hitPoint = info.interPoint;
	Vec3 viewDir = rayDir;
	Vec3 lightPos = scene.lights[0]->getCenter();
	Vec3 normal = info.normal;
	Vec3 brdfParameter = info.surface->diffuse;

	objNum = info.objNum;

	Vec3 color[2] = { Vec3::zeros(), Vec3::zeros() };
	rep(p, mcptSample) {
		std::pair<Vec3, Vec3> radiance = tracer->color(ray, rng);
		color[0] += Vec3{ cut(radiance.first[0]), cut(radiance.first[1]), cut(radiance.first[2]) } * (1.0 / mcptSample);
		color[1] += Vec3{ cut(radiance.second[0]), cut(radiance.second[1]), cut(radiance.second[2]) } * (1.0 / mcptSample);
	}

	return{ { hitPoint, viewDir, lightPos, normal, brdfParameter, color[0] }, { hitPoint, viewDir, lightPos, normal, brdfParameter, color[1] } };
}

void DataGenerator::generateTrainData() {
	vector<ofstream *> direct_fouts, indirect_fouts;
	rep(k, allObjNum) {
		ostringstream oss;
		oss << "../data/direct_data_" << k << ".txt";
		direct_fouts.push_back(new ofstream(oss.str()));
		oss.str("");
		oss << "../data/indirect_data_" << k << ".txt";
		indirect_fouts.push_back(new ofstream(oss.str()));
	}

	unsigned long long threadCnt = omp_get_max_threads();
	vector<RNGenerator *> rngs{ threadCnt };
	rep(i, threadCnt) rngs[i] = new RNGenerator(int(time(NULL)) ^ i);

	#pragma omp parallel for
	for (int i = 0; i < viewPointCnt; ++i) {
		RNGenerator *rng = rngs[omp_get_thread_num()];

		int dim[3];
		dim[0] = i / sqr(viewPointDim);
		dim[1] = (i - dim[0] * sqr(viewPointDim)) / viewPointDim;
		dim[2] = i % viewPointDim;

		Vec3 viewPoint = getRandomViewPoint(*rng, dim, viewPointDim);

		omp_set_lock(&printLock);
		cout << "Handling viewPoint " << i << ": " << viewPoint << " . . ." << endl;
		omp_unset_lock(&printLock);

		if (!viewPoint.isFinite()) continue;

		for (int j = 0; j < rayCnt; ++j) {
			Vec3 rayDir = getRandomRayDir(*rng);

			int objNum;
			auto dataPair = getSingleTrainData(rng, viewPoint, rayDir, objNum);

			if (objNum >= 0) {
				omp_set_lock(&printLock);
				outputTrainData(*direct_fouts[objNum], dataPair.first);
				outputTrainData(*indirect_fouts[objNum], dataPair.second);
				omp_unset_lock(&printLock);
			}
		}
	}
}