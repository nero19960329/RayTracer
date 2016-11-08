#include "Mesh.h"

using namespace std;

shared_ptr<Intersect> Mesh::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared <MeshIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

bool MeshIntersect::isIntersect() const {
	rep(idx, mesh.tris.size()) {
		if (mesh.tris[idx].intersect(ray, info)) {
			intersectFaceIdx = idx;
			return true;
		}
	}
	return false;
}

real_t MeshIntersect::getDistToInter() const {
	if (isfinite(distToInter)) return distToInter;

	distToInter = info.distToInter;
	repa(idx, intersectFaceIdx + 1, mesh.tris.size()) {
		TriIntersectInfo tmpInfo;
		if (mesh.tris[idx].intersect(ray, tmpInfo) && info.distToInter > tmpInfo.distToInter) {
			info = tmpInfo;
			intersectFaceIdx = idx;
		}
	}

	distToInter = info.distToInter;
	return distToInter;
}

Vec3 MeshIntersect::getNormal() const {
	Vec3 normal;
	if (mesh.normals.size()) {
		array<Vec3, 3> normals = { mesh.normals[mesh.triIndices[intersectFaceIdx][0]], mesh.normals[mesh.triIndices[intersectFaceIdx][1]], mesh.normals[mesh.triIndices[intersectFaceIdx][2]] };
		normal = (1 - info.u - info.v) * normals[0] + info.u * normals[1] + info.v * normals[2];
	} else {
		normal = mesh.tris[intersectFaceIdx].getNormal();
	}

	return normal;
}

shared_ptr<Surface> MeshIntersect::getInterPointSurfaceProperty() const {
	return nullptr;
}