#include "Triangle.h"

using namespace std;

shared_ptr<Intersect> Triangle::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared<TriangleIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

bool TriangleIntersect::isIntersect() const {
	Vec3 e1 = tri.b - tri.a, e2 = tri.c - tri.a;
	Vec3 s = ray.orig - tri.a;
	Vec3 tmpVec1 = ray.dir.cross(e2);
	Vec3 tmpVec2 = s.cross(e1);

	real_t a = tmpVec1.dot(e1);
	real_t u, v;
	if (a > epsilon) {
		u = tmpVec1.dot(s);
		if (u < 0 || u > a) return false;
		v = tmpVec2.dot(ray.dir);
		if (v < 0 || u + v > a) return false;
	} else if (a < -epsilon) {
		u = tmpVec1.dot(s);
		if (u > 0 || u < a) return false;
		v = tmpVec2.dot(ray.dir);
		if (v > 0 || u + v < a) return false;
	} else {
		return false;
	}

	distToInter = tmpVec2.dot(e2);
	if ((distToInter < 0) ^ (a < 0)) return false;
	distToInter /= a;

	return true;
}

real_t TriangleIntersect::getDistToInter() const {
	return distToInter;
}

Vec3 TriangleIntersect::getNormal() const {
	normal = (tri.b - tri.a).cross(tri.c - tri.b).getNormalized();
	if (ray.dir.dot(normal) < 0) return normal;
	else return -normal;
}

shared_ptr<Surface> TriangleIntersect::getInterPointSurfaceProperty() const {
	assert(false);
	return nullptr;
}