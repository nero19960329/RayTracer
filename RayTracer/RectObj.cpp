#include "RectObj.h"

using namespace std;

shared_ptr<Intersect> RectObj::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared<RectIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

bool RectIntersect::isIntersect() const {
	if (!PlaneIntersect::isIntersect()) return false;
	
	distToInter = -projOrigToInter / projDir;
	Vec3 interCoor = ray.getDistPoint(distToInter) - rectObj.rectangle.center;
	if (fabs(interCoor.dot(rectObj.rectangle.x)) <= rectObj.rectangle.radius &&
		fabs(interCoor.dot(rectObj.rectangle.y)) <= rectObj.rectangle.radius) return true;
	else return false;
}

real_t RectIntersect::getDistToInter() const {
	return distToInter;
}

Vec3 RectIntersect::getNormal() const {
	return PlaneIntersect::getNormal();
}