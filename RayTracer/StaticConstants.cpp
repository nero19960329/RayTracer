#include "Constants.h"
#include "Material.h"
#include "Vec3.h"

const Vec3 Vec3::NONE{ 0, 0, 0 };
const Vec3 Vec3::BLACK{ epsilon, epsilon, epsilon };
const Vec3 Vec3::WHITE{ 1.0 - epsilon, 1.0 - epsilon, 1.0 - epsilon };
const Vec3 Vec3::RED{ 1.0 - epsilon, epsilon, epsilon };
const Vec3 Vec3::GREEN{ epsilon, 1.0 - epsilon, epsilon };
const Vec3 Vec3::BLUE{ epsilon, epsilon, 1.0 - epsilon };
const Vec3 Vec3::YELLOW{ 1.0 - epsilon, 1.0 - epsilon, epsilon };
const Vec3 Vec3::CYAN{ epsilon, 1.0 - epsilon, 1.0 - epsilon };
const Vec3 Vec3::MAGENTA{ 1.0 - epsilon, epsilon, 1.0 - epsilon };

const Material Material::FLOOR{ 0.0, 0.1, 5 };
const Material Material::MIRROR{ 0.0, 0.2, 5, 1.0 };
const Material Material::A_BIT_MIRROR{ 0.0, 0.2, 5, 0.3 };
const Material Material::TRANSPARENT_MATERIAL{ 0.0, 0.0, 0, 0.0, 1.0 };
const Material Material::PLASTIC{};