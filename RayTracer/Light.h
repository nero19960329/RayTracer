#pragma once

#include "Constants.h"
#include "Sphere.h"
#include "Vec3.h"

#include <iostream>

class Light : public Sphere {
public:
	Vec3 color;
	real_t intensity;

public:
	Light() {}
	Light(const Vec3 &center, const Vec3 &_color, real_t _intensity, real_t _radius):
		Sphere(std::make_shared<PureTexture>(Material{ 0.0, 0.0, 0.0, 0.0, 0.0, _color * _intensity }, _color), center, _radius), color(_color), intensity(_intensity) {}
	~Light() {}

	Vec3 getCenter() const { return pureSphere.center; }
	void setCenter(const Vec3 &_center) { pureSphere.center = _center; }
	void setRadius(real_t _radius) { pureSphere.radius = _radius; }
};