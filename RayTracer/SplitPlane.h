#pragma once

#include "Utils.h"

class SplitPlane {
public:
	int axis;
	real_t value;

public:
	SplitPlane() {}
	SplitPlane(int _axis, real_t _value) :
		axis(_axis), value(_value) {}
	~SplitPlane() {}
};