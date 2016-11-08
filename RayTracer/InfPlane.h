#pragma once

#include "Utils.h"
#include "Vec3.h"

class InfPlane {
public:
	Vec3 normal;
	real_t offset;

public:
	InfPlane() {}
	explicit InfPlane(const Vec3 &_normal) : normal(_normal) {}
	InfPlane(const Vec3 &_normal, real_t _offset) : normal(_normal), offset(_offset) {}
	~InfPlane() {}
};