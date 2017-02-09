#pragma once

#include "Renderer.h"

class TravelManager {
private:
	real_t step = 5.0;
	real_t thetaStep = step * 0.02 * PI;

public:
	TravelManager() {}
	~TravelManager() {}

	void travel(Renderer &renderer) const;
};