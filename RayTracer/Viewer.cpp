#include "Viewer.h"

#include <iostream>

using namespace std;

Ray Viewer::getRay_RT(int i, int j, int p, int q) const {
	Vec3 tmpVec = LT - deltaH * j + deltaW * i - deltaH * q / antiSample + deltaW * p / antiSample;
	Vec3 rayCenter, rayDir;

	if (antialiasingFlag) {
		tmpVec -= erand48() * deltaH / antiSample;
		tmpVec += erand48() * deltaW / antiSample;
	}

	if (!dopFlag) {
		rayCenter = center;
		rayDir = tmpVec - rayCenter;
	} else {
		Vec3 focusPoint = center + (tmpVec - center).getNormalized() * focusOffset;
		real_t tmpRadius = erand48() * apertureSize;
		real_t tmpTheta = erand48() * 360.0;
		rayCenter = center + tmpRadius * (cos(tmpTheta) * x + sin(tmpTheta) * y);
		rayDir = focusPoint - rayCenter;
	}

	if (projFlag == ORTHO) {
		rayCenter = tmpVec;
		rayDir = dir;
	}

	return{ rayCenter, rayDir.getNormalized() };
}

Ray Viewer::getRay_MCPT(int i, int j, int p, int q) const {
	Vec3 tmpVec = LT - deltaH * j + deltaW * i - deltaH * q * 0.5 + deltaW * p * 0.5;
	tmpVec -= erand48() * deltaH;
	tmpVec += erand48() * deltaW;

	return{ center, (tmpVec - center).getNormalized() };
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

void Viewer::changeProjectionType() {
	if (projFlag == PERSPECTIVE) projFlag = ORTHO;
	else {
		if (viewport.width && viewport.height) projFlag = PERSPECTIVE;
		else return;
	}
	computeVariables();
}

void Viewer::computeVariables() {
	up = dir.cross(up).cross(dir).getNormalized();

	real_t w2, h2;
	if (projFlag == PERSPECTIVE) {
		h2 = tan(fovy * PI / 360);
		w2 = h2 * screen.getRatio();
	} else {
		w2 = viewport.width * 0.5;
		h2 = viewport.height * 0.5;
	}
	x = dir.cross(up);
	y = up;
	Vec3 halfW, halfH;
	halfW = w2 * x;
	halfH = h2 * y;
	LT = center + dir - halfW + halfH;
	deltaW = (halfW + halfW) / screen.width;
	deltaH = (halfH + halfH) / screen.height;
}