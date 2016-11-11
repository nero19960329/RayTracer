#pragma once

#include "Utils.h"

enum PlaneSide { LEFT_SIDE, RIGHT_SIDE, NONE_SIDE };

class SplitPlane {
public:
	int axis;
	real_t value;
	PlaneSide side;

public:
	SplitPlane() {}
	SplitPlane(int _axis, real_t _value, PlaneSide _side) :
		axis(_axis), value(_value), side(_side) {}
	~SplitPlane() {}

	bool operator == (const SplitPlane &plane) const {
		return axis == plane.axis && value == plane.value && side == plane.side;
	}
};