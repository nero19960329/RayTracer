#pragma once

#include <glm.hpp>

#include "ray.h"
#include "rng.h"

class RaySampler;

class Camera {
	friend class RaySampler;

public:
	int width, height;
	glm::dvec3 center, dir, up;
	double fovy;

protected:
	glm::dvec3 LT, deltaW, deltaH;
	glm::dvec3 x, y;

public:
	Camera(int width_, int height_, glm::dvec3 center_, glm::dvec3 target, glm::dvec3 up_, double fovy_);
};

class RaySampler {
private:
	const Camera & cam;
	mutable RNG rng;

	mutable int jitterIdx;

public:
	bool rtMode, mcptMode;
	bool jitterMode, dopMode;
	int jitterSampleNum, dopSampleNum, mcptSampleNum;
	double apertureSize, focusOffset;	// for depth of field

public:
	RaySampler(const Camera & camera_);

	Ray ray(int i, int j) const;
	Ray jitterRay(int i, int j) const;
	Ray dopRay(int i, int j) const;
	Ray randomRay(int i, int j) const;
};