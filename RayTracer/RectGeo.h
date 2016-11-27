#pragma once

#include "Utils.h"
#include "Vec3.h"

class RectGeo {
public:
	Vec3 a, b, c, d;
	Vec3 center, x, y;
	real_t radius;

public:
	RectGeo() {}
	RectGeo(const Vec3 &_center, const Vec3 &_x, const Vec3 &_y, real_t _radius) :
		center(_center), radius(_radius) {
		x = _x.getNormalized();
		y = _x.cross(_y).cross(_x).getNormalized();
		a = _center - _radius * x + _radius * y;
		b = _center - _radius * x - _radius * y;
		c = _center + _radius * x - _radius * y;
		d = _center + _radius * x + _radius * y;
	}
};