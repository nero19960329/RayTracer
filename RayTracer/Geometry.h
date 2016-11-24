#pragma once

#include "Utils.h"

class Geometry {
public:
	int width, height;

public:
	explicit Geometry(std::pair<int, int> size) : width(size.first), height(size.second) {}
	Geometry(int _width, int _height) : width(_width), height(_height) {}
	~Geometry() {}

	real_t getRatio() const {
		return width * 1.0 / height;
	}
};