#pragma once

#include <glm.hpp>

template <typename T>
inline bool updateMin(T &value, const T &min) {
	if (value > min) {
		value = min;
		return true;
	}
	return false;
}

template <typename T>
inline bool updateMax(T &value, const T &max) {
	if (value < max) {
		value = max;
		return true;
	}
	return false;
}

inline glm::dvec3 clamp(glm::dvec3 v) {
	glm::dvec3 r = v;
	if (r.x < 0.0) r.x = 0.0;
	else if (r.x > 1.0) r.x = 1.0;
	if (r.y < 0.0) r.y = 0.0;
	else if (r.y > 1.0) r.y = 1.0;
	if (r.z < 0.0) r.z = 0.0;
	else if (r.z > 1.0) r.z = 1.0;
	return r;
}