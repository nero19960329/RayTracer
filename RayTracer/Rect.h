#pragma once

#include "Object.h"
#include "Face.h"

#include <array>

class RectIntersect;

class RectObj : public Object {
	friend class RectIntersect;

protected:
	Vec3 a, b, c, d, center;
	real_t radius;
	std::array<Face, 2> faces;

public:
	RectObj() : Object(nullptr) {}
	RectObj(const std::shared_ptr<Texture> &_texture, const Vec3 &_center, const Vec3 &_x, const Vec3 &_y, real_t _radius) :
		Object(_texture), center(_center), radius(_radius) {
		Vec3 x = _x.getNormalized();
		Vec3 y = _x.cross(_y).cross(_x).getNormalized();
		a = center - radius * x + radius * y;
		b = center - radius * x - radius * y;
		c = center + radius * x - radius * y;
		d = center + radius * x + radius * y;
		faces[0] = Face{ _texture, a, b, c };
		faces[1] = Face{ _texture, c, d, a };
	}

	AABB getAABB() const override {
		AABB res;
		res.expand(faces[0].getAABB());
		res.expand(faces[1].getAABB());
		return res;
	}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;

	bool liesInPlane(const SplitPlane &p) const override {
		return (fabs(a[p.axis] - p.value) < epsilon &&
			fabs(b[p.axis] - p.value) < epsilon &&
			fabs(c[p.axis] - p.value) < epsilon &&
			fabs(d[p.axis] - p.value) < epsilon);
	}
};

class RectIntersect : public Intersect {
private:
	const RectObj &rect;
	std::array<std::shared_ptr<FaceIntersect>, 2> intersects;

	mutable int intersectIdx = -1;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const override { return nullptr; }

	const Object *getObj() const override { return &rect; }

public:
	RectIntersect(const RectObj &_rect, const Ray &_ray) :
		Intersect(_ray), rect(_rect) {
		intersects[0] = std::make_shared<FaceIntersect>(rect.faces[0], ray);
		intersects[1] = std::make_shared<FaceIntersect>(rect.faces[1], ray);
	}

	bool isIntersect() const override {
		if (intersects[0]->isIntersect()) intersectIdx = 0;
		else if (intersects[1]->isIntersect()) intersectIdx = 1;
		else return false;
		return true;
	}

	real_t getDistToInter() const override {
		distToInter = intersects[intersectIdx]->getDistToInter();
		return distToInter;
	}

	Vec3 getNormal() const override { return intersects[intersectIdx]->getNormal(); }
};