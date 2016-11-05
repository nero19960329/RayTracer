#pragma once

#include <Windows.h>

#include "Utils.h"

class Timer {
private:
	long long freq;
	long long start, end;

public:
	Timer() {
		LARGE_INTEGER tmp;
		QueryPerformanceFrequency(&tmp);
		freq = tmp.QuadPart;
	}
	~Timer() {}

	void begin() {
		LARGE_INTEGER tmp;
		QueryPerformanceCounter(&tmp);
		start = tmp.QuadPart;
	}

	real_t getDuring() {
		LARGE_INTEGER tmp;
		QueryPerformanceCounter(&tmp);
		end = tmp.QuadPart;
		return (end - start) * 1.0 / freq;
	}
};