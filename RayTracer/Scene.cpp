#include "Scene.h"

using namespace std;

shared_ptr<Shader> Scene::getShader(ShaderType type) const {
	if (type == PHONG) {
		return make_shared<Phong>(*this);
	} else {
		return nullptr;
	}
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
		if (intersect) {
			return true;
		}
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
				res = std::move(intersect);
			}
		}
	}

	return res;
}