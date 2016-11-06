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

const Material Material::FLOOR{ 0.0, 0.1, 5 };
const Material Material::MIRROR{ 0.0, 0.2, 5, 1.0 };
const Material Material::A_BIT_MIRROR{ 0.0, 0.2, 5, 0.3 };
const Material Material::TRANSPARENT_MATERIAL{ 0.0, 0.0, 0, 0.0, 1.0 };
const Material Material::PLASTIC{};