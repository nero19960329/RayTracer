#pragma once

#include <Windows.h>

#include "Utils.h"

#undef max
#undef min

class Timer {
private:
	long long freq;
	long long start, end;

public:
	Timer() {
		LARGE_INTEGER tmp;
		QueryPerformanceFrequency(&tmp);
		freq = tmp.QuadPart;

		begin();
	}
	~Timer() {}

	void begin() {
		LARGE_INTEGER tmp;
		QueryPerformanceCounter(&tmp);
		start = tmp.QuadPart;
	}

	real_t getDuration() {
		LARGE_INTEGER tmp;
		QueryPerformanceCounter(&tmp);
		end = tmp.QuadPart;
		return (end - start) * 1.0 / freq;
	}
};