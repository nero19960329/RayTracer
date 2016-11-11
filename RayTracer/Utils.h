#pragma once

#include <cstdio>
#include <iostream>

#include <algorithm>
#include <functional>
#include <vector>

using real_t = double;

const real_t epsilon = 1e-6;
const real_t biggerEpsilon = epsilon * 3;
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

template <typename T> 
inline typename std::vector<T>::iterator findKthElement(std::vector<T> &vec, int k, std::function<bool(const T &, const T &)> cmp) {
	auto low = vec.begin(), high = vec.end();
	while (low != high) {
		const T &tmp = *low;
		auto p = std::stable_partition(low, high, std::bind(cmp, std::placeholders::_1, tmp));
		auto delta = p - vec.begin() + 1;
		if (delta < k) low = p + 1;
		else if (delta > k) high = p;
		else return p;
	}
}

__declspec(noreturn) inline void error_exit(const char *msg) {
	printf("\n%s", msg);
	exit(0);
}