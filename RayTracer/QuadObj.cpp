#include "QuadObj.h"

using namespace std;

shared_ptr<Intersect> QuadObj::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared<QuadIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

bool QuadIntersect::isIntersect() const {
	const Vec3 &V00 = quadObj.quad.a, &V01 = quadObj.quad.b, &V11 = quadObj.quad.c, &V10 = quadObj.quad.d;
	Vec3 E01 = V10 - V00, E03 = V01 - V00;
	Vec3 P = ray.dir.cross(E03);
	real_t det = E01.dot(P);
	if (fabs(det) < epsilon) {
		return false;
	}
	Vec3 T = ray.orig - V00;
	real_t alpha = T.dot(P) / det;
	if (alpha < 0) {
		return false;
	}
	Vec3 Q = T.cross(E01);
	real_t beta = ray.dir.dot(Q) / det;
	if (beta < 0) {
		return false;
	}

	if (alpha + beta > 1) {
		Vec3 E23 = V01 - V11, E21 = V10 - V11;
		Vec3 P_ = ray.dir.cross(E21);
		real_t det_ = E23.dot(P_);
		if (fabs(det_) < epsilon) {
			return false;
		}
		Vec3 T_ = ray.orig - V11;
		real_t alpha_ = T_.dot(P_) / det_;
		if (alpha_ < 0) {
			return false;
		}
		Vec3 Q_ = T_.cross(E23);
		real_t beta_ = ray.dir.dot(Q_) / det_;
		if (beta_ < 0) {
			return false;
		}
	}

	real_t t = E03.dot(Q) / det;
	if (t < 0) {
		return false;
	}

	Vec3 E02 = V11 - V00;
	Vec3 N = E01.cross(E03);
	normal = N.getNormalized();
	real_t alpha_11, beta_11;
	if (fabs(N.x) >= fabs(N.y) && fabs(N.x) >= fabs(N.z)) {
		alpha_11 = (E02.y * E03.z - E02.z * E03.y) / N.x;
		beta_11 = (E01.y * E02.z - E01.z * E02.y) / N.x;
	} else if (fabs(N.y) >= fabs(N.x) && fabs(N.y) > fabs(N.z)) {
		alpha_11 = (E02.z * E03.x - E02.x * E03.z) / N.y;
		beta_11 = (E01.z * E02.x - E01.x * E02.z) / N.y;
	} else {
		alpha_11 = (E02.x * E03.y - E02.y * E03.x) / N.z;
		beta_11 = (E01.x * E02.y - E01.y * E02.x) / N.z;
	}

	real_t u, v;
	if (fabs(alpha_11 - 1) < epsilon) {
		u = alpha;
		if (fabs(beta_11 - 1) < epsilon) v = beta;
		else v = beta / (u * (beta_11 - 1) + 1);
	} else if (fabs(beta_11 - 1) < epsilon) {
		v = beta;
		u = alpha / (v * (alpha_11 - 1) + 1);
	} else {
		real_t A, B, C, DELTA;
		A = -(beta_11 - 1);
		B = alpha * (beta_11 - 1) - beta * (alpha_11 - 1) - 1;
		C = alpha;
		DELTA = sqr(B) - 4 * A * C;
		real_t Q = -0.5 * (B + sqrt(DELTA) * sgn(B));
		u = Q / A;
		if (u < 0 || u > 1) u = C / Q;
		v = beta / (u * (beta_11 - 1) + 1);
	}

	Vec3 interPoint = (1 - u) * (1 - v) * V00 + u * (1 - v) * V10 + u * v * V11 + (1 - u) * v * V01;
	distToInter = (interPoint - ray.orig).norm();

	return true;
}

real_t QuadIntersect::getDistToInter() const {
	return distToInter;
}

Vec3 QuadIntersect::getNormal() const {
	if (ray.dir.dot(normal) < 0) return normal;
	else return -normal;
}