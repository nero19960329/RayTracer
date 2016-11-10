#pragma once

#include "Utils.h"

#include <cmath>
#include <fstream>
#include <limits>

class Vec3 {
public:
	union {
		struct {
			real_t vec[3];
		};
		struct {
			real_t x, y, z;
		};
	};

public:
	Vec3() {}
	Vec3(real_t _x, real_t _y, real_t _z) : x(_x), y(_y), z(_z) {}
	Vec3(const Vec3 &v) : x(v.x), y(v.y), z(v.z) {}
	~Vec3() {}

	Vec3 &operator = (const Vec3 &v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	// overload operators
	Vec3 operator - () const {
		return{ -x, -y, -z };
	}

	real_t operator [] (int k) const {
		return vec[k];
	}

	real_t &operator [] (int k) {
		return vec[k];
	}

	Vec3 operator + (const Vec3 &v) const {
		return{ x + v.x, y + v.y, z + v.z };
	}

	Vec3 &operator += (const Vec3 &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vec3 operator - (const Vec3 &v) const {
		return{ x - v.x, y - v.y, z - v.z };
	}

	Vec3 &operator -= (const Vec3 &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vec3 operator * (real_t k) const {
		return{ x * k, y * k, z * k };
	}

	friend inline Vec3 operator * (real_t k, const Vec3 &v) {
		return v * k;
	}

	Vec3 &operator *= (real_t k) {
		x *= k;
		y *= k;
		z *= k;
		return *this;
	}

	Vec3 operator / (real_t k) const {
		return *this * (1.0 / k);
	}

	Vec3 operator /= (real_t k) {
		real_t t = 1.0 / k;
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	bool operator == (const Vec3 &v) const {
		return fabs(x - v.x) < epsilon && fabs(y - v.y) < epsilon && fabs(z - v.z) < epsilon;
	}

	bool operator != (const Vec3 &v) const {
		return fabs(x - v.x) >= epsilon || fabs(y - v.y) >= epsilon && fabs(z - v.z) >= epsilon;
	}

	bool operator < (const Vec3 &v) const {
		return x < v.x + epsilon && y < v.y + epsilon && z < v.z + epsilon;
	}

	bool operator > (const Vec3 &v) const {
		return x > v.x - epsilon && y > v.y - epsilon && z > v.z - epsilon;
	}

	friend std::ostream &operator << (std::ostream &os, const Vec3 &v) {
		return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	}

	// other operation
	inline real_t dot(const Vec3 &v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	inline Vec3 cross(const Vec3 &v) const {
		return{ y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x };
	}

	inline Vec3 mul(const Vec3 &v) const {
		return{ x * v.x, y * v.y, z * v.z };
	}

	inline real_t sqr() const {
		return ::sqr(x) + ::sqr(y) + ::sqr(z);
	}

	inline real_t norm() const {
		return sqrt(sqr());
	}

	inline Vec3 reflection(const Vec3 &normal) const {
		return (normal + normal) * normal.dot(*this) - *this;
	}

	inline Vec3 refraction(const Vec3 &normal, real_t eta) const {
		real_t tmp = 1.0 / eta;
		real_t cos1 = dot(normal);
		if (1 - ::sqr(cos1) > ::sqr(eta)) {
			return Vec3::zeros();
		}
		real_t cos2 = sqrt(1 - (1 - ::sqr(cos1)) * ::sqr(tmp));
		return -(*this) * tmp - (cos2 - cos1 * tmp) * normal;
	}

	inline void normalize() {
		real_t t = 1.0 / norm();
		x *= t;
		y *= t;
		z *= t;
	}

	inline Vec3 getNormalized() const {
		real_t t = 1.0 / norm();
		return{ x * t, y * t, z * t };
	}

	inline bool isZero() const {
		return fabs(x) < epsilon && fabs(y) < epsilon && fabs(z) < epsilon;
	}

	inline bool isFinite() const {
		return isfinite(x) && isfinite(y) && isfinite(z);
	}

	// static methods
	static Vec3 zeros() {
		return{ 0.0, 0.0, 0.0 };
	}

	static Vec3 ones() {
		return{ 1.0, 1.0, 1.0 };
	}

	static Vec3 epsilons() {
		return{ epsilon, epsilon, epsilon };
	}

	static Vec3 min() {
		return{ std::numeric_limits<real_t>::min(), std::numeric_limits<real_t>::min(), std::numeric_limits<real_t>::min() };
	}

	static Vec3 max() {
		return{ std::numeric_limits<real_t>::max(), std::numeric_limits<real_t>::max(), std::numeric_limits<real_t>::max() };
	}

	static Vec3 infinity() {
		return{ std::numeric_limits<real_t>::infinity(), std::numeric_limits<real_t>::infinity(), std::numeric_limits<real_t>::infinity() };
	}

	// static constants
	static const Vec3 BLACK, WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA;
};