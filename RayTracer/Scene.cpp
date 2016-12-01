#include "MatReader.h"
#include "MonteCarloPathTracing.h"
#include "NeuralNetwork.h"
#include "Scene.h"
#include "RayTracing.h"

#include <sstream>

using namespace std;

shared_ptr<TraceBase> Scene::getTracer(TraceType traceType, BRDFType brdfType) const {
	if (traceType == RT) {
		vector<NeuralNetwork> directNNVec, indirectNNVec;
		rep(i, 7) {
			ostringstream oss;
			oss << "../nn/direct_" << (i + 1) << ".mat";
			MatReader reader(oss.str());
			directNNVec.push_back(NeuralNetwork(reader));
			oss.str("");
			oss << "../nn/indirect_" << (i + 1) << ".mat";
			reader = MatReader(oss.str());
			indirectNNVec.push_back(NeuralNetwork(reader));
		}
		return make_shared<RayTracing>(*this, directNNVec, indirectNNVec);
	} else if (traceType == MCPT) return make_shared<MonteCarloPathTracing>(*this, lights, brdfType);
	return nullptr;
}

void Scene::addLight(const shared_ptr<Light> &light) {
	lights.push_back(light);
}

void Scene::addObject(const shared_ptr<Object> &object) {
	objs.push_back(object);
}

bool Scene::isIntersect(const Ray &ray, real_t dist) const {
	for (const auto &obj : objs) {
		auto intersect = obj->getTrace(ray, dist);
		if (intersect) return true;
	}
	return false;
}

shared_ptr<Intersect> Scene::getIntersect(const Ray &ray) const {
	shared_ptr<Intersect> res = nullptr;
	real_t minDist = std::numeric_limits<real_t>::max();

	for (const auto &obj : objs) {
		auto intersect = obj->getTrace(ray);
		if (intersect) {
			real_t dis = intersect->getDistToInter();
			if (updateMin(minDist, dis)) res = intersect;
		}
	}

	for (const auto &light : lights) {
		auto intersect = light->getTrace(ray);
		if (intersect) {
			real_t dis = intersect->getDistToInter();
			if (updateMin(minDist, dis)) res = intersect;
		}
	}

	return res;
}

bool Scene::isInnerPoint(const Vec3 &p) const {
	Vec3 dir{ 0.0, 1.0, 0.0 };
	Ray testRay{ p, dir };
	auto intersect = getIntersect(testRay);
	if (!intersect) return true;

	IntersectInfo info = intersect->getIntersectInfo();
	return intersect->getObj()->hasInside();
}