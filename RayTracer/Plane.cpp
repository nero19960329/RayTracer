#include "Plane.h"

using namespace std;

shared_ptr<Intersect> Plane::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared<PlaneIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

bool PlaneIntersect::isIntersect() const {
	projDir = ray.dir.dot(plane.infPlane.normal);
	if (fabs(projDir) < epsilon) {	// parallel to plane
		return false;
	} else {
		projOrigToInter = ray.orig.dot(plane.infPlane.normal) - plane.infPlane.offset;
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
	if (ray.dir.dot(plane.infPlane.normal) < 0) return plane.infPlane.normal;
	else return -plane.infPlane.normal;
}

shared_ptr<Surface> PlaneIntersect::getInterPointSurfaceProperty() const {
	Vec3 x{ -plane.infPlane.normal.y, plane.infPlane.normal.x, 0 };
	Vec3 y{ 0, -plane.infPlane.normal.z, plane.infPlane.normal.y };
	if (x.isZero()) {
		x = { -plane.infPlane.normal.z, 0, plane.infPlane.normal.x };
	}

	return plane.getTexture()->getSurfaceProperty(interPoint.dot(x), interPoint.dot(y));
}