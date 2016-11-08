#pragma once

#include <array>
#include <memory>
#include <vector>

#include "Mesh.h"
#include "Triangle.h"
#include "Vec3.h"

class ObjReader {
private:
	std::vector<Vec3> vertexNormals;
	std::vector<Vec3> vertices;
	std::vector<std::array<int, 3>> triIndices;
	std::vector<Tri> tris;

	Vec3 center;
	real_t radius;

public:
	explicit ObjReader(const std::string &objFileName, const Vec3 &_center = Vec3::zeros(), real_t _radius = 1.0);
	~ObjReader() {}

	std::shared_ptr<Mesh> getMesh();
	void setCenter(const Vec3 &_center) {
		center = _center;
	}
	void setRadius(real_t _radius) {
		radius = _radius;
	}

private:
	Vec3 readVec3(const std::string &str);
	std::array<int, 3> readTri(const std::string &str);

	void fitSize();
	void moveToCenter();
	void scaleToRadius();
};