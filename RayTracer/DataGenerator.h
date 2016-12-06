#pragma once

#include "BRDF.h"
#include "RNG.h"
#include "Scene.h"
#include "TraceBase.h"
#include "Utils.h"
#include "Vec3.h"

#include <omp.h>

class DataGenerator {
private:
	int viewPointCnt, viewPointDim, rayCnt, mcptSample;
	int allObjNum;
	const Scene &scene;

	Vec3 bounds[2];

	std::shared_ptr<TraceBase> tracer;

	omp_lock_t printLock;

public:
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
	DataGenerator(const Scene &_scene, int _viewPointDim, int _rayCnt, int _mcptSample, int _allObjNum, const Vec3 &minBound, const Vec3 &maxBound) :
		scene(_scene), viewPointDim(_viewPointDim), viewPointCnt(_viewPointDim * _viewPointDim * _viewPointDim), rayCnt(_rayCnt), mcptSample(_mcptSample), allObjNum(_allObjNum) {
		bounds[0] = minBound;
		bounds[1] = maxBound;

		tracer = scene.getTracer(MCPT, LAMBERTIAN);

		omp_init_lock(&printLock);
	}

	void generateTrainData();

private:
	inline Vec3 getRandomViewPoint(RNGenerator &rng, int dims[3], int dim) const {
		int cnt = 0;
		while (cnt < 100) {
			Vec3 res;
			rep(k, 3) res[k] = ((dims[k] + rng.randomReal()) * 1.0 / dim) * (bounds[1][k] - bounds[0][k]) + bounds[0][k];
			if (!scene.isInnerPoint(res)) return res;
			++cnt;
		}
		return Vec3::infinity();
	}

	inline Vec3 getRandomRayDir(RNGenerator &rng) const {
		real_t theta = 2 * PI * rng.randomReal();
		real_t u = 2 * rng.randomReal() - 1;
		return{ sqrt(1 - sqr(u)) * cos(theta), sqrt(1 - sqr(u)) * sin(theta), u };
	}

	std::pair<TrainData, TrainData> getSingleTrainData(RNGenerator *rng, const Vec3 &viewPoint, const Vec3 &rayDir, int &objNum) const;

	inline void outputTrainData(std::ofstream &os, const TrainData &trainData) const {
		TrainData data = trainData;
		os << data.hitPoint << " "
			<< data.viewDir << " "
			<< data.lightPos << " "
			<< data.normal << " "
			<< data.brdfParameter << " "
			<< data.color << std::endl;
	}
};