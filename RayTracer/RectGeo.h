#pragma once

#include "Utils.h"
#include "Vec3.h"

class RectGeo {
public:
	Vec3 a, b, c, d;
	Vec3 center, x, y;
	real_t length_x, length_y;

public:
	RectGeo() {}
	RectGeo(const Vec3 &_center, const Vec3 &_x, const Vec3 &_y, real_t _length_x, real_t _length_y) :
		center(_center), length_x(_length_x), length_y(_length_y) {
		x = _x.getNormalized();
		y = _x.cross(_y).cross(_x).getNormalized();
		a = center - length_x * x + length_y * y;
		b = center - length_x * x - length_y * y;
		c = center + length_x * x - length_y * y;
		d = center + length_x * x + length_y * y;
	}

	real_t area() const { return 4 * length_x * length_y; }
};