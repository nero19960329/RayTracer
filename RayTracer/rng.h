#pragma once

#include <random>
#include <vector>

class RNG {
private:
	std::uniform_real_distribution<double> uni;
	std::mt19937_64 eng;

public:
	explicit RNG(int seed = 1234) : eng(seed) {}

	void setSeed(int seed) { eng = std::mt19937_64(seed); }
	double randomDouble() { return uni(eng); }
	int randomIdx(const std::vector<double> & cdf);
};