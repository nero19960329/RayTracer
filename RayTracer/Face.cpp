#include "Face.h"

using namespace std;

shared_ptr<Intersect> Face::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared<FaceIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

bool FaceIntersect::isIntersect() const {
	bool isIntersect = face.tri.intersect(ray, info);
	distToInter = info.distToInter;
	return isIntersect;
}

real_t FaceIntersect::getDistToInter() const {
	return distToInter;
}

Vec3 FaceIntersect::getNormal() const {
	if (face.smoothShadingFlag) {
		normal = (1 - info.u - info.v) * face.normals[0] + info.u * face.normals[1] + info.v * face.normals[2];
		return normal;
	} else {
		normal = face.tri.getNormal();
		if (ray.dir.dot(normal) < 0) return normal;
		else return -normal;
	}
}

shared_ptr<Surface> FaceIntersect::getInterPointSurfaceProperty() const {
	assert(false);
	return nullptr;
}