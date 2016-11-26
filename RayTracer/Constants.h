#pragma once

#include "Vec3.h"

const Vec3 AMBIENT_INTENSITY{ 0.2, 0.2, 0.2 };
const real_t DEFAULT_AMBIENT = 0.05;
const real_t DEFAULT_SPECULAR = 0.2;
const real_t DEFAULT_SHININESS = 5.0;

const real_t DEFAULT_ATTENUATION_COEFFICIENT = 2e-2;

const int MAX_RAY_TRACING_DEPTH = 5;
const int MIN_MONTE_CARLO_PATH_TRACING_DEPTH = 10;
const real_t RUSSIAN_ROULETTE_POSSIBILITY = 0.5;

const real_t VACUUM_REFRACTION_INDEX = 1.0;
const real_t WATER_REFRACTION_INDEX = 1.333;
const real_t GLASS_REFRACTION_INDEX = 1.5;

const int MIN_KDNODE_OBJS = 100;
const int MAX_KDTREE_DEPTH = 100;

const real_t KDTREE_SAH_KT = 0.75;
const real_t KDTREE_SAH_KI = 1.0;