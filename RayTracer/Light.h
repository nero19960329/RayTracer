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
	Light() {}
	Light(const RectGeo &rect, const Vec3 &_color, real_t _intensity) :
		RectObj(std::make_shared<PureTexture>(Material{ 0.0, 0.0, 0.0, 0.0, 0.0, _color * _intensity }, _color), rect), color(_color), intensity(_intensity) {}
	~Light() {}

	Vec3 getCenter() const { return rectangle.center; }
	void setCenter(const Vec3 &_center) { rectangle.center = _center; }
};