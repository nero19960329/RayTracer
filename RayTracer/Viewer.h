#pragma once

#include "Geometry.h"
#include "Ray.h"
#include "Vec3.h"

#include <vector>

class SceneReader;

class Viewer {
	friend SceneReader;

private:
	Geometry screen;
	Vec3 center, dir, up;
	real_t fovy;

protected:
	Viewer() : screen{ 0, 0 }, center{ 0.0, 0.0, 0.0 } {}

public:
	Viewer(Geometry _screen, Vec3 _center, Vec3 _target, Vec3 _up, real_t _fovy) :
		screen(_screen), center(_center), dir(_target - _center), up(_up), fovy(_fovy) {
		dir.normalize();
		up.normalize();
		up = dir.cross(up).cross(dir);
	}
	~Viewer() {}

	std::vector<std::vector<Ray>> getRayVector() const;
	Geometry getScreen() const {
		return screen;
	}
};