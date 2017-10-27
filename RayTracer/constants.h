#pragma once

#include <glm.hpp>

const double eps = 1e-6;
const double PI = 3.14159265359;
const double INV_PI = 1.0 / PI;

const glm::dvec3 AMBIENT_INTENSITY{ 0.2, 0.2, 0.2 };
const double DEFAULT_AMBIENT = 0.05;
const double DEFAULT_SPECULAR = 0.2;
const double DEFAULT_SHININESS = 5.0;
const double DEFAULT_ATTENUATION_COEFFICIENT = 1e-4;
const double ABSORB_COEFFICIENT = 0.0;

const int MIN_KDNODE_OBJS = 10;
const int MAX_KDTREE_DEPTH = 100;
const double KDTREE_SAH_KT = 0.75;
const double KDTREE_SAH_KI = 1.0;

const int MAX_RAY_TRACING_DEPTH = 1;
const int MIN_BIDIRECTIONAL_PATH_TRACING_DEPTH = 5;
const int MIN_MONTE_CARLO_PATH_TRACING_DEPTH = 5;
const double RUSSIAN_ROULETTE_POSSIBILITY = 0.5;

const double VACUUM_REFRACTION_INDEX = 1.0;
const double WATER_REFRACTION_INDEX = 1.333;
const double GLASS_REFRACTION_INDEX = 1.5;

const glm::dvec3 inf_vec3{ 
	std::numeric_limits<double>::infinity(),
	std::numeric_limits<double>::infinity(),
	std::numeric_limits<double>::infinity()
};
const glm::dvec3 max_vec3{
	std::numeric_limits<double>::max(),
	std::numeric_limits<double>::max(),
	std::numeric_limits<double>::max()
};
const glm::dvec3 eps_vec3{ eps, eps, eps };
const glm::dvec3 zero_vec3{ 0.0, 0.0, 0.0 };
const glm::dvec3 one_vec3{ 1.0, 1.0, 1.0 };