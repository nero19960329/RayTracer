#pragma once

#include "Vec3.h"

const Vec3 AMBIENT_INTENSITY{ 0.2, 0.2, 0.2 };
const real_t DEFAULT_AMBIENT = 0.05;
const real_t DEFAULT_SPECULAR = 0.2;
const real_t DEFAULT_SHININESS = 5.0;

const real_t AIR_BEER_DENSITY = 2e-2;

const int MAX_TRACING_DEPTH = 5;

const real_t VACUUM_REFRACTION_INDEX = 1.0;
const real_t WATER_REFRACTION_INDEX = 1.333;
const real_t GLASS_REFRACTION_INDEX = 1.5;

const int MIN_KDNODE_OBJS = 10;
const real_t DUPLICATE_RATIO = 0.5;