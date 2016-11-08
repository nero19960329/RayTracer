#pragma once

#include "Utils.h"
#include "Vec3.h"

class PureSphere {
public:
	Vec3 center;
	real_t radius;

public:
	PureSphere() {}
	PureSphere(const Vec3 &_center, real_t _radius) :
		center(_center), radius(_radius) {}
	~PureSphere() {}
};