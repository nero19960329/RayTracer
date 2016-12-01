#pragma once

#include "RNG.h"
#include "Scene.h"

class TraceBase {
protected:
	const Scene &scene;

public:
	TraceBase(const Scene &_scene) : scene(_scene) {}
	virtual ~TraceBase() {}

	virtual std::pair<Vec3, Vec3> color(const Ray &ray, RNGenerator *rng = nullptr) const = 0;
};
