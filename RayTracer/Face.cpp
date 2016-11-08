#include "Face.h"

using namespace std;

shared_ptr<Intersect> Face::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared<FaceIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

bool FaceIntersect::isIntersect() const {
	return face.tri.intersect(ray, distToInter);
}

real_t FaceIntersect::getDistToInter() const {
	return distToInter;
}

Vec3 FaceIntersect::getNormal() const {
	normal = face.tri.getNormal();
	if (ray.dir.dot(normal) < 0) return normal;
	else return -normal;
}

shared_ptr<Surface> FaceIntersect::getInterPointSurfaceProperty() const {
	assert(false);
	return nullptr;
}