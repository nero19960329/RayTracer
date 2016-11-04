#pragma once

#include "Utils.h"

class Geometry {
public:
	int width, height;

public:
	Geometry(int _width, int _height) : width(_width), height(_height) {}
	~Geometry() {}

	real_t getRatio() const {
		return width * 1.0 / height;
	}
};