#include "Constants.h"
#include "Material.h"
#include "Vec3.h"

const Vec3 Vec3::BLACK{ 0.0, 0.0, 0.0 };
const Vec3 Vec3::WHITE{ 1.0, 1.0, 1.0 };
const Vec3 Vec3::RED{ 1.0, 0.0, 0.0 };
const Vec3 Vec3::GREEN{ 0.0, 1.0, 0.0 };
const Vec3 Vec3::BLUE{ 0.0, 0.0, 1.0 };
const Vec3 Vec3::YELLOW{ 1.0, 1.0, 0.0 };
const Vec3 Vec3::CYAN{ 0.0, 1.0, 1.0 };
const Vec3 Vec3::MAGENTA{ 1.0, 0.0, 1.0 };

/*const Surface Surface::BLACK{ DEFAULT_AMBIENT, Vec3::BLACK, DEFAULT_SPECULAR, DEFAULT_SHININESS, Vec3::BLACK };
const Surface Surface::WHITE{ DEFAULT_AMBIENT, Vec3::WHITE, DEFAULT_SPECULAR, DEFAULT_SHININESS, Vec3::BLACK };
const Surface Surface::RED{ DEFAULT_AMBIENT, Vec3::RED, DEFAULT_SPECULAR, DEFAULT_SHININESS, Vec3::BLACK };
const Surface Surface::GREEN{ DEFAULT_AMBIENT, Vec3::GREEN, DEFAULT_SPECULAR, DEFAULT_SHININESS, Vec3::BLACK };
const Surface Surface::BLUE{ DEFAULT_AMBIENT, Vec3::BLUE, DEFAULT_SPECULAR, DEFAULT_SHININESS, Vec3::BLACK, 0.25, 0.0 };
const Surface Surface::YELLOW{ DEFAULT_AMBIENT, Vec3::YELLOW, DEFAULT_SPECULAR, DEFAULT_SHININESS, Vec3::BLACK };
const Surface Surface::CYAN{ DEFAULT_AMBIENT, Vec3::CYAN, DEFAULT_SPECULAR, DEFAULT_SHININESS, Vec3::BLACK };
const Surface Surface::MAGENTA{ DEFAULT_AMBIENT, Vec3::MAGENTA, DEFAULT_SPECULAR, DEFAULT_SHININESS, Vec3::BLACK };*/

const Material Material::FLOOR{ 0.0, 0.1, 5 };
const Material Material::MIRROR{ 0.0, 0.2, 5, 1.0, 0.0 };
const Material Material::A_BIT_MIRROR{ 0.0, 0.2, 5, 0.3, 0.0 };
const Material Material::PLASTIC{};