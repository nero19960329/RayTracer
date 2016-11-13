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