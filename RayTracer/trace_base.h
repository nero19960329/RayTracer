#pragma once

#include "RNG.h"
#include "Scene.h"

class TraceBase {
protected:
	const Scene & scene;

public:
	TraceBase(const Scene & scene_) : scene(scene_) {}

	virtual glm::dvec3 color(const Ray &ray, RNG * rng = nullptr) const = 0;
};