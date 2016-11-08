#include "Mesh.h"

using namespace std;

shared_ptr<Intersect> Mesh::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared <MeshIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

bool MeshIntersect::isIntersect() const {
	for (const auto &tri : mesh.tris) {
		auto intersect = tri.getTrace(ray);
		if (intersect) return true;
	}
	return false;
}

real_t MeshIntersect::getDistToInter() const {
	if (isfinite(distToInter)) return distToInter;

	distToInter = numeric_limits<real_t>::max();
	for (const auto &tri : mesh.tris) {
		auto intersect = tri.getTrace(ray);
		if (intersect && distToInter > intersect->getDistToInter()) {
			distToInter = intersect->getDistToInter();
			nearestIntersect = intersect;
		}
	}
	return distToInter;
}

Vec3 MeshIntersect::getNormal() const {
	return nearestIntersect->getNormal();
}

shared_ptr<Surface> MeshIntersect::getInterPointSurfaceProperty() const {
	return nullptr;
}