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
	return (interPoint - sphere.center).getNormalized();
}