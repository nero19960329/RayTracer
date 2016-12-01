#pragma once

#include "Utils.h"

#include <random>

class RNGenerator {
private:
	std::uniform_real_distribution<real_t> uni;
	std::mt19937_64 eng;

public:
	RNGenerator(int seed = 1111) : eng(seed) {}

	void setSeed(int seed) { eng = std::mt19937_64(seed); }

	real_t randomReal() { return uni(eng); }
};