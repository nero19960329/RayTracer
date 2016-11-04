#pragma once

#include <cassert>
#include <cstdio>

using real_t = double;

const real_t epsilon = 1e-6;
const real_t PI = 3.14159265359;

#define rep(x, y) for (auto x = decltype(y){0}; x != y; ++x)
#define repa(x, y, z) for (auto x = decltype(z){y}; x != z; ++x)
#define repd(x, y, z) for (auto x = decltype(z){y}; x != z; --x)

// left-closed and right-open
template <typename T>
inline bool betw(const T &x, const T &y, const T &z) {
	return x >= y && x < z;
}

template <typename T>
inline T sqr(const T &x) {
	return x * x;
}

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

inline void printProgress(real_t p) {
	printf("\r%.2lf%%", p * 100);
}