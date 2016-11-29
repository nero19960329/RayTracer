#pragma once

#include "Constants.h"
#include "RectObj.h"
#include "Vec3.h"

#include <iostream>

class Light : public RectObj {
public:
	Vec3 color;
	real_t intensity;

public:
	Light() : RectObj() {}
	Light(const RectGeo &rect, const Vec3 &_color, real_t _intensity) :
		RectObj(std::make_shared<PureTexture>(Material{ 0.0, 0.0, 0.0, 0.0, 0.0, _color * _intensity }, _color), -1, rect), color(_color), intensity(_intensity) {}
	~Light() {}

	Vec3 getCenter() const { return rectangle.center; }
	void setCenter(const Vec3 &_center) { rectangle.center = _center; }

	void luminaireSample(const Vec3 &interPoint, Vec3 &outDir) const {
		real_t u = erand48() * rectangle.length_x * 2, v = erand48() * rectangle.length_y * 2;
		Vec3 target = rectangle.a + u * rectangle.x - v * rectangle.y;
		outDir = (target - interPoint).getNormalized();
	}

	real_t area() const { return rectangle.area(); }

	std::array<Vec3, 4> vertices() const {
		std::array<Vec3, 4> res = { rectangle.a, rectangle.b, rectangle.c, rectangle.d };
		return res;
	}

	real_t v1crossv2() const {
		return (2 * rectangle.length_x * rectangle.x).cross(2 * rectangle.length_y * rectangle.y).norm();
	}
};