#pragma once

#include "Geometry.h"
#include "Ray.h"
#include "Vec3.h"

#include <vector>

class Viewer {
private:
	Geometry screen;
	Vec3 center, dir, up;
	real_t fovy;

public:
	Viewer(Geometry _screen, Vec3 _center, Vec3 _dir, Vec3 _up, real_t _fovy) :
		screen(_screen), center(_center), dir(_dir), up(_up), fovy(_fovy) {}
	~Viewer() {}

	std::vector<std::vector<Ray>> getRayVector() const;
	Geometry getScreen() const {
		return screen;
	}
};