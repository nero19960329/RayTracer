#pragma once

#include "BRDF.h"
#include "Scene.h"
#include "TraceBase.h"
#include "Utils.h"
#include "Vec3.h"

class DataGenerator {
private:
	int viewPointCnt, rayCnt, mcptSample;
	int allObjNum;
	const Scene &scene;

	Vec3 bounds[2];

	std::shared_ptr<TraceBase> tracer;

	struct TrainData {
		Vec3 hitPoint, viewDir, lightPos, normal;
		Vec3 brdfParameter;
		Vec3 color;

		void normalize(const Vec3 &minBound, const Vec3 &maxBound) {
			rep(k, 3) {
				hitPoint[k] = (hitPoint[k] - minBound[k]) / (maxBound[k] - minBound[k]);
				lightPos[k] = (lightPos[k] - minBound[k]) / (maxBound[k] - minBound[k]);
			}
		}
	};

public:
	DataGenerator(const Scene &_scene, int _viewPointCnt, int _rayCnt, int _mcptSample, int _allObjNum, const Vec3 &minBound, const Vec3 &maxBound) :
		scene(_scene), viewPointCnt(_viewPointCnt), rayCnt(_rayCnt), mcptSample(_mcptSample), allObjNum(_allObjNum) {
		bounds[0] = minBound;
		bounds[1] = maxBound;

		tracer = scene.getTracer(MCPT, LAMBERTIAN);
	}

	void generateTrainData() const;

private:
	inline Vec3 getRandomViewPoint() const {
		while (true) {
			Vec3 res;
			rep(k, 3) res[k] = erand48() * (bounds[1][k] - bounds[0][k]) + bounds[0][k];
			if (!scene.isInnerPoint(res)) return res;
		}
	}

	inline Vec3 getRandomRayDir() const {
		real_t theta = 2 * PI * erand48();
		real_t u = 2 * erand48() - 1;
		return{ sqrt(1 - sqr(u)) * cos(theta), sqrt(1 - sqr(u)) * sin(theta), u };
	}

	std::pair<TrainData, TrainData> getSingleTrainData(const Vec3 &viewPoint, const Vec3 &rayDir, int &objNum) const;

	inline void outputTrainData(std::ofstream &os, const TrainData &trainData) const {
		TrainData data = trainData;
		//data.normalize(bounds[0], bounds[1]);
		os << data.hitPoint << " "
			<< data.viewDir << " "
			<< data.lightPos << " "
			<< data.normal << " "
			<< data.brdfParameter << " "
			<< data.color << std::endl;
	}
};