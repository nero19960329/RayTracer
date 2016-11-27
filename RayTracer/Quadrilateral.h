#pragma once

#include "Utils.h"
#include "Vec3.h"

class Quadrilateral {
public:
	Vec3 a, b, c, d;

public:
	Quadrilateral() {}
	Quadrilateral(const Vec3 &_a, const Vec3 &_b, const Vec3 &_c, const Vec3 &_d) :
		a(_a), b(_b), c(_c), d(_d) {}
};