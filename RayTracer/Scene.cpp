#include "Scene.h"

using namespace std;

shared_ptr<TraceBase> Scene::getTracingType(TraceType type) const {
	if (type == RAY_TRACING) {
		return make_shared<RayTracing>(*this);
	}
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
			if (updateMin(minDist, dis)) {
				res = intersect;
			}
		}
	}

	return res;
}