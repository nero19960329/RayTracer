#include "Rect.h"

using namespace std;

shared_ptr<Intersect> RectObj::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared<RectIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}