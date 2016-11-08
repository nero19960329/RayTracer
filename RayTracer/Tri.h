#pragma once

#include "Ray.h"
#include "Utils.h"
#include "Vec3.h"

struct TriIntersectInfo {
	real_t distToInter, u, v;
};

class Tri {
public:
	Vec3 a, b, c;

public:
	Tri() {}
	Tri(const Vec3 &_a, const Vec3 &_b, const Vec3 &_c) :
		a(_a), b(_b), c(_c) {}
	~Tri() {}

	bool intersect(const Ray &ray, TriIntersectInfo &info) const {
		real_t f;
		if (!intersectRay(ray, f, info.distToInter, info.u, info.v)) return false;
		info.distToInter *= f;
		info.u *= f;
		info.v *= f;
		return true;
	}

	bool intersect(const Ray &ray, real_t &distToInter) const {
		real_t f, u, v;
		if (!intersectRay(ray, f, distToInter, u, v)) return false;
		distToInter *= f;
		return true;
	}

	bool intersect(const Ray &ray) const {
		real_t f, distToInter, u, v;
		return intersectRay(ray, f, distToInter, u, v);
	}

	Vec3 getNormal() const {
		return (b - a).cross(c - b).getNormalized();
	}

private:
	bool intersectRay(const Ray &ray, real_t &f, real_t &distToInter, real_t &u, real_t &v) const {
		Vec3 e1 = b - a, e2 = c - a;
		Vec3 s = ray.orig - a;
		Vec3 tmpVec1 = ray.dir.cross(e2);
		Vec3 tmpVec2 = s.cross(e1);

		real_t tmp = tmpVec1.dot(e1);
		if (tmp > epsilon) {
			u = tmpVec1.dot(s);
			if (u < 0 || u > tmp) return false;
			v = tmpVec2.dot(ray.dir);
			if (v < 0 || u + v > tmp) return false;
		} else if (tmp < -epsilon) {
			u = tmpVec1.dot(s);
			if (u > 0 || u < tmp) return false;
			v = tmpVec2.dot(ray.dir);
			if (v > 0 || u + v < tmp) return false;
		} else {
			return false;
		}

		distToInter = tmpVec2.dot(e2);
		if ((distToInter < 0) ^ (tmp < 0)) return false;
		f = 1.0 / tmp;

		return true;
	}
};