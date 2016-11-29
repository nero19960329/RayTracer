#include "DataGenerator.h"

#include <fstream>
#include <vector>

using namespace std;

pair<DataGenerator::TrainData, DataGenerator::TrainData> DataGenerator::getSingleTrainData(const Vec3 &viewPoint, const Vec3 &rayDir, int &objNum) const {
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
		std::pair<Vec3, Vec3> radiance = tracer->color(ray);
		color[0] += radiance.first * (1.0 / mcptSample);
		color[1] += radiance.second * (1.0 / mcptSample);
	}

	return{ { hitPoint, viewDir, lightPos, normal, brdfParameter, color[0] }, { hitPoint, viewDir, lightPos, normal, brdfParameter, color[1] } };
}

void DataGenerator::generateTrainData() const {
	vector<ofstream *> direct_fouts, indirect_fouts;
	rep(k, allObjNum) {
		ostringstream oss;
		oss << "../data/direct_data_" << k << ".txt";
		direct_fouts.push_back(new ofstream(oss.str()));
		oss.clear();
		oss << "../data/indirect_data_" << k << ".txt";
		indirect_fouts.push_back(new ofstream(oss.str()));
	}

	for (int i = 0; i < viewPointCnt; ++i) {
		Vec3 viewPoint = getRandomViewPoint();
		cout << "viewPoint: " << viewPoint << endl;

		for (int j = 0; j < rayCnt; ++j) {
			Vec3 rayDir = getRandomRayDir();
			//cout << "rayDir: " << rayDir << endl;

			int objNum;
			auto dataPair = getSingleTrainData(viewPoint, rayDir, objNum);

			if (objNum >= 0) {
				outputTrainData(*direct_fouts[objNum], dataPair.first);
				outputTrainData(*indirect_fouts[objNum], dataPair.second);
				/*cout << "ID: " << objNum << endl;
				cout << "hitPoint: " << dataPair.first.hitPoint << endl;
				cout << "viewDir: " << dataPair.first.viewDir << endl;
				cout << "lightPos: " << dataPair.first.lightPos << endl;
				cout << "normal: " << dataPair.first.normal << endl;
				cout << "brdf: " << dataPair.first.brdfParameter << endl;
				cout << "direct: " << dataPair.first.color << endl;
				cout << "indirect: " << dataPair.second.color << endl;

				getchar();*/
			}
		}
	}
}