#include "Viewer.h"

#include <iostream>
#include "Timer.h"

using namespace std;

const real_t invRANDMAX = 1.0 / RAND_MAX;

Ray Viewer::getRay(int i, int j) const {
	Vec3 tmpVec = LT - deltaH * (j + 0.5) + deltaW * (i + 0.5);
	Vec3 rayCenter, rayDir;

	if (!dopFlag && !antialiasingFlag) {
		rayCenter = center;
		rayDir = tmpVec - rayCenter;
	} else if (!dopFlag && antialiasingFlag) {
		rayCenter = center;
		tmpVec -= (rand() - 0.5) * deltaH * invRANDMAX;
		tmpVec += (rand() - 0.5) * deltaW * invRANDMAX;
		rayDir = tmpVec - rayCenter;
	} else if (dopFlag && !antialiasingFlag) {
		Vec3 focusPoint = center + (tmpVec - center).getNormalized() * focusOffset;
		real_t tmpRadius = rand() * apertureSize * invRANDMAX;
		real_t tmpTheta = rand() * 360.0 * invRANDMAX;
		rayCenter = center + tmpRadius * (cos(tmpTheta) * x + sin(tmpTheta) * y);
		rayDir = focusPoint - rayCenter;
	} else {
		tmpVec -= (rand() - 0.5) * deltaH * invRANDMAX;
		tmpVec += (rand() - 0.5) * deltaW * invRANDMAX;
		Vec3 focusPoint = center + (tmpVec - center).getNormalized() * focusOffset;
		real_t tmpRadius = rand() * apertureSize * invRANDMAX;
		real_t tmpTheta = rand() * 360.0 * invRANDMAX;
		rayCenter = center + tmpRadius * (cos(tmpTheta) * x + sin(tmpTheta) * y);
		rayDir = focusPoint - rayCenter;
	}

	if (projFlag == ORTHO) {
		rayCenter = tmpVec;
		rayDir = dir;
	}

	return{ rayCenter, rayDir.getNormalized() };
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