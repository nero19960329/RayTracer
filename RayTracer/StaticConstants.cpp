#include "Surface.h"
#include "Vec3.h"

const Vec3 Vec3::BLACK{ 0.0, 0.0, 0.0 };
const Vec3 Vec3::WHITE{ 1.0, 1.0, 1.0 };
const Vec3 Vec3::RED{ 1.0, 0.0, 0.0 };
const Vec3 Vec3::GREEN{ 0.0, 1.0, 0.0 };
const Vec3 Vec3::BLUE{ 0.0, 0.0, 1.0 };
const Vec3 Vec3::YELLOW{ 1.0, 1.0, 0.0 };
const Vec3 Vec3::CYAN{ 0.0, 1.0, 1.0 };
const Vec3 Vec3::MAGENTA{ 1.0, 0.0, 1.0 };

const Surface Surface::BLACK{ 0.2, Vec3::BLACK, 0.2, 5, Vec3::BLACK };
const Surface Surface::WHITE{ 0.2, Vec3::WHITE, 0.2, 5, Vec3::BLACK };
const Surface Surface::RED{ 0.2, Vec3::RED, 0.2, 5, Vec3::BLACK };
const Surface Surface::GREEN{ 0.2, Vec3::GREEN, 0.2, 5, Vec3::BLACK };
const Surface Surface::BLUE{ 0.2, Vec3::BLUE, 0.2, 5, Vec3::BLACK };
const Surface Surface::YELLOW{ 0.2, Vec3::YELLOW, 0.2, 5, Vec3::BLACK };
const Surface Surface::CYAN{ 0.2, Vec3::CYAN, 0.2, 5, Vec3::BLACK };
const Surface Surface::MAGENTA{ 0.2, Vec3::MAGENTA, 0.2, 5, Vec3::BLACK };