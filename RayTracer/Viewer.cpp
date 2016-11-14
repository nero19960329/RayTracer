#include "Viewer.h"

#include <iostream>
#include "Timer.h"

using namespace std;

Ray Viewer::getRay(int i, int j) const {
	Vec3 tmpVec = LT - deltaH * (j + 0.5) + deltaW * (i + 0.5);
	if (!dopFlag && !antialiasingFlag) {
		return{ center, (tmpVec - center).getNormalized() };
	} else if (!dopFlag && antialiasingFlag) {
		tmpVec -= (rand() - 0.5) * deltaH / RAND_MAX;
		tmpVec += (rand() - 0.5) * deltaW / RAND_MAX;
		return{ center, (tmpVec - center).getNormalized() };
	} else if (dopFlag && !antialiasingFlag) {
		Vec3 focusPoint = center + (tmpVec - center).getNormalized() * focusOffset;
		real_t tmpRadius = rand() * apertureSize / RAND_MAX;
		real_t tmpTheta = rand() * 360.0 / RAND_MAX;
		Vec3 tmpCenter = center + tmpRadius * (cos(tmpTheta) * x + sin(tmpTheta) * y);
		return { tmpCenter, (focusPoint - tmpCenter).getNormalized() };
	} else {
		tmpVec -= (rand() - 0.5) * deltaH / RAND_MAX;
		tmpVec += (rand() - 0.5) * deltaW / RAND_MAX;
		Vec3 focusPoint = center + (tmpVec - center).getNormalized() * focusOffset;
		real_t tmpRadius = rand() * apertureSize / RAND_MAX;
		real_t tmpTheta = rand() * 360.0 / RAND_MAX;
		Vec3 tmpCenter = center + tmpRadius * (cos(tmpTheta) * x + sin(tmpTheta) * y);
		return { tmpCenter, (focusPoint - tmpCenter).getNormalized() };
	}
}

void Viewer::moveForward(real_t delta) {
	center += dir * delta;
	computeVariables();
}

void Viewer::moveSide(real_t delta) {
	center += x * delta;
	computeVariables();
}

void Viewer::rotateUp(real_t theta) {
	Vec3 newDir = dir * cos(theta) + y * sin(theta);
	dir = newDir;
	computeVariables();
}

void Viewer::rotateSide(real_t theta) {
	Vec3 newDir = dir * cos(theta) + x * sin(theta);
	dir = newDir;
	computeVariables();
}

void Viewer::computeVariables() {
	up = dir.cross(up).cross(dir).getNormalized();

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
}