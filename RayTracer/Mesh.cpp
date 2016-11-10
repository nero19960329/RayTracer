#include "Mesh.h"

using namespace std;

shared_ptr<Intersect> Mesh::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared<MeshIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

bool MeshIntersect::isIntersect() const {
	return intersect->isIntersect();
}

real_t MeshIntersect::getDistToInter() const {
	distToInter = intersect->getDistToInter();
	return distToInter;
}

Vec3 MeshIntersect::getNormal() const {
	return intersect->getNormal();
}

shared_ptr<Surface> MeshIntersect::getInterPointSurfaceProperty() const {
	return nullptr;
}