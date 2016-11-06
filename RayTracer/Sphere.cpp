#include "Sphere.h"

using namespace std;

shared_ptr<Intersect> Sphere::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared<SphereIntersect>(*this, ray);
	if (res->isIntersect()) {
		if (res->getDistToInter() < dist) {
			return res;
		}
	}
	return nullptr;
}

bool SphereIntersect::isIntersect() const {
	real_t distToCenter2 = (ray.orig - sphere.center).sqr();
	if (distToCenter2 < sqr(sphere.radius)) {
		return true;
	}

	distToProjCenter = (sphere.center - ray.orig).dot(ray.dir);
	if (distToProjCenter < 0) {
		return false;
	}

	halfCord2 = sqr(sphere.radius) - distToCenter2 + sqr(distToProjCenter);
	if (halfCord2 < 0) {
		return false;
	}

	return true;
}

real_t SphereIntersect::getDistToInter() const {
	if (isfinite(distToInter)) {
		return distToInter;
	}

	distToInter = distToProjCenter - sqrt(halfCord2);
	interPoint = Intersect::getIntersection();
	return distToInter;
}

Vec3 SphereIntersect::getNormal() const {
	normal = (interPoint - sphere.center).getNormalized();
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