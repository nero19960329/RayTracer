#pragma once

#include "Constants.h"
#include "Rect.h"
#include "Vec3.h"

#include <iostream>

class Light : public RectObj {
public:
	Vec3 color;
	real_t intensity;

public:
	Light() {}
	Light(const Vec3 &center, const Vec3 &_color, real_t _intensity, const Vec3 &x, const Vec3 &y, real_t _radius):
		RectObj(std::make_shared<PureTexture>(Material{ 0.0, 0.0, 0.0, 0.0, 0.0, _color * _intensity }, _color), center, x, y, _radius), color(_color), intensity(_intensity) {}
	~Light() {}

	Vec3 getCenter() const { return center; }
	void setCenter(const Vec3 &_center) { center = _center; }
	void setRadius(real_t _radius) { radius = _radius; }
};