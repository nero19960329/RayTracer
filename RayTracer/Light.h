#pragma once

#include "Constants.h"
#include "Sphere.h"
#include "Vec3.h"

#include <iostream>

class Light : public Sphere {
public:
	Vec3 color;
	real_t a, b, c;	// intensity = 1.0 / (a + b * dist + c * dist * dist)

public:
	Light(const Vec3 &center, const Vec3 &_color, real_t _a = INTENSITY_DECAY_A, real_t _b = INTENSITY_DECAY_B, real_t _c = INTENSITY_DECAY_C):
		Sphere(nullptr, center, 0.1), color(_color), a(_a), b(_b), c(_c) {}
	~Light() {}

	Vec3 getIntensity(real_t dist) const {
		return color / (a + b * dist + c * dist * dist);
	}

	Vec3 getCenter() const {
		return center;
	}
};