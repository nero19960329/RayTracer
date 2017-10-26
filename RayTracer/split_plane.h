#pragma once

enum PlaneSide { LEFT_SIDE, RIGHT_SIDE, NONE_SIDE };

class SplitPlane {
public:
	int axis;
	double value;
	PlaneSide side;

public:
	SplitPlane() {}
	SplitPlane(int axis_, double value_, PlaneSide side_) :
		axis(axis_), value(value_), side(side_) {}

	bool operator == (const SplitPlane &plane) const {
		return axis == plane.axis && value == plane.value && side == plane.side;
	}
};