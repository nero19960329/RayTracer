#pragma once

#include "Scene.h"

class TraceBase {
protected:
	const Scene &scene;

public:
	TraceBase(const Scene &_scene) : scene(_scene) {}
	virtual ~TraceBase() {}

	virtual std::pair<Vec3, Vec3> color(const Ray &ray) const = 0;
};
