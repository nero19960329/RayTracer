#include "Plane.h"

using namespace std;

shared_ptr<Intersect> Plane::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared<PlaneIntersect>(*this, ray);
	if (res->isIntersect()) {
		if (res->getDistToInter() < dist) {
			return res;
		}
	}
	return nullptr;
}

bool PlaneIntersect::isIntersect() const {
	projDir = ray.dir.dot(plane.normal);
	if (fabs(projDir) < epsilon) {	// parallel to plane
		return false;
	} else {
		projOrigToInter = ray.orig.dot(plane.normal) - plane.offset;
		if ((projOrigToInter > 0) ^ (projDir > 0)) {
			return true;
		} else {
			return false;
		}
	}
}

real_t PlaneIntersect::getDistToInter() const {
	distToInter = -projOrigToInter / projDir;
	return distToInter;
}

Vec3 PlaneIntersect::getNormal() const {
	return plane.normal;
}