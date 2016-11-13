#pragma once

#include "Geometry.h"
#include "Ray.h"
#include "Vec3.h"

#include <cstdlib>
#include <ctime>
#include <vector>

class SceneReader;

class Viewer {
	friend SceneReader;

public:
	Geometry screen;
	Vec3 center, dir, up;
	real_t fovy;

	bool dopFlag = false;
	bool antialiasingFlag = false;

	real_t apertureSize = 0.1, focusOffset;
	int dopSample = 1, antiSample = 1;

private:
	Vec3 LT, deltaW, deltaH;
	Vec3 x, y;

protected:
	Viewer() : screen{ 0, 0 }, center{ 0.0, 0.0, 0.0 } {}

public:
	Viewer(Geometry _screen, Vec3 _center, Vec3 _target, Vec3 _up, real_t _fovy) :
		screen(_screen), center(_center), dir(_target - _center), up(_up), fovy(_fovy) {
		dir.normalize();
		up.normalize();
		up = dir.cross(up).cross(dir);

		real_t w2, h2;
		h2 = tan(fovy * PI / 360);
		w2 = h2 * screen.getRatio();
		x = dir.cross(up);
		y = up;
		Vec3 halfW, halfH;
		halfW = w2 * x;
		halfH = h2 * y;
		LT = center + dir - halfW + halfH;
		deltaW = (halfW + halfW) / screen.width;
		deltaH = (halfH + halfH) / screen.height;

		srand((unsigned) time(0));
	}
	~Viewer() {}

	Ray getRay(int i, int j) const;
	Geometry getScreen() const {
		return screen;
	}
};