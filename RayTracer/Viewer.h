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
	enum PROJ_TYPE { ORTHO, PERSPECTIVE };

	Geometry screen, viewport;
	Vec3 center, dir, up;
	real_t fovy;

	PROJ_TYPE projFlag;
	bool dopFlag, antialiasingFlag;

	real_t apertureSize, focusOffset;
	int dopSample = 1, antiSample = 1;

private:
	Vec3 LT, deltaW, deltaH;
	Vec3 x, y;

protected:
	Viewer() : screen{ 0, 0 }, viewport{ 0, 0 }, center{ 0.0, 0.0, 0.0 } {}

public:
	Viewer(Geometry _screen, Vec3 _center, Vec3 _target, Vec3 _up, real_t _fovy, 
		bool _dop = false, real_t _aperture = 0.1, real_t _offset = 0.0, int _dopSample = 1, 
		bool _anti = false, int _antiSample = 1, Geometry _viewport = Geometry{ 0, 0 }) :
		screen(_screen), center(_center), dir(_target - _center), up(_up), fovy(_fovy),
		dopFlag(_dop), apertureSize(_aperture), focusOffset(_offset), dopSample(_dopSample), 
		antialiasingFlag(_anti), antiSample(_antiSample), viewport(_viewport) {
		dir.normalize();
		up.normalize();

		if (viewport.width && viewport.height) projFlag = ORTHO;
		else projFlag = PERSPECTIVE;

		computeVariables();
	}
	~Viewer() {}

	void setAntiThings(int _antiSample) { antialiasingFlag = true; antiSample = _antiSample; }
	void setDopThings(real_t _aperture, real_t _offset, int _dopSample) { dopFlag = true; apertureSize = _aperture; focusOffset = _offset; dopSample = _dopSample; }
	void setViewPort(Geometry _viewport = Geometry{ 0, 0 }) { viewport = _viewport; }

	Ray getRay(int i, int j, int p, int q) const;
	Geometry getScreen() const {
		return screen;
	}

	void moveForward(real_t delta);
	void moveSide(real_t delta);
	void rotateUp(real_t delta);
	void rotateSide(real_t delta);

	void changeProjectionType();

	void computeVariables();
};