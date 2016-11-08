#include "Sphere.h"

using namespace std;

shared_ptr<Intersect> Sphere::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared<SphereIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

bool SphereIntersect::isIntersect() const {
	real_t distToCenter2 = (ray.orig - sphere.pureSphere.center).sqr();
	inside = distToCenter2 < sqr(sphere.pureSphere.radius);

	distToProjCenter = (sphere.pureSphere.center - ray.orig).dot(ray.dir);
	toward = distToProjCenter > 0;

	if (!inside && !toward) {
		return false;
	}

	halfCord2 = sqr(sphere.pureSphere.radius) - distToCenter2 + sqr(distToProjCenter);
	if (!inside && halfCord2 < 0) {
		return false;
	}

	return true;
}

real_t SphereIntersect::getDistToInter() const {
	if (isfinite(distToInter)) {
		return distToInter;
	}

	if (!inside) distToInter = distToProjCenter - sqrt(halfCord2);
	else if (toward) distToInter = distToProjCenter + sqrt(halfCord2);
	else distToInter = sqrt(halfCord2) - distToProjCenter;

	interPoint = Intersect::getIntersection();
	return distToInter;
}

Vec3 SphereIntersect::getNormal() const {
	if (!inside) normal = (interPoint - sphere.pureSphere.center).getNormalized();
	else normal = (sphere.pureSphere.center - interPoint).getNormalized();

	return normal;
}

shared_ptr<Surface> SphereIntersect::getInterPointSurfaceProperty() const {
	if (fabs(interPoint.x) < epsilon && fabs(interPoint.y) < epsilon) {
		return sphere.getTexture()->getSurfaceProperty(0, 0);
	} else {
		real_t u, v;
		u = atan2(normal.y, normal.x) / (PI + PI);
		// cuz (u+1.0)-floor(u+1.0) = u-floor(u), so omit it
		if ((normal.x < 0) ^ (normal.y > 0)) {
			u += 0.5;
		}
		v = asin(normal.z) / (PI + PI);
		return sphere.getTexture()->getSurfaceProperty(u, v);
	}
}

real_t SphereIntersect::getNextRefractionIndex() const {
	if (inside) return VACUUM_REFRACTION_INDEX;
	else return sphere.innerRefrIdx;
}