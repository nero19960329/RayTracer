#pragma once

#include "Renderer.h"

class TravelManager {
private:
	real_t step = 0.5;
	real_t thetaStep = step * 0.04 * PI;

public:
	TravelManager() {}
	~TravelManager() {}

	void travel(Renderer &renderer) const;
};