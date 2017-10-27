#include "rng.h"

int RNG::randomIdx(const std::vector<double> & cdf) {
	double r = randomDouble();
	for (int i = 0; i < cdf.size() - 1; ++i) {
		if (cdf[i] <= r && r <= cdf[i + 1]) return i;
	}
	return -1;
}
