#pragma once

#include "Vec3.h"

const Vec3 AMBIENT_INTENSITY{ 0.2, 0.2, 0.2 };
const real_t DEFAULT_AMBIENT = 0.05;
const real_t DEFAULT_SPECULAR = 0.2;
const real_t DEFAULT_SHININESS = 5.0;

const real_t AIR_BEER_DENSITY = 2e-2;

const int MAX_TRACING_DEPTH = 5;