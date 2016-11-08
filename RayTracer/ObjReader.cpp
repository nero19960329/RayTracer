#include "ObjReader.h"
#include "Utils.h"

using namespace std;

ObjReader::ObjReader(const string &objFileName, const Vec3 &_center, real_t _radius) :
	center(_center), radius(_radius) {
	ifstream fin(objFileName);
	if (!fin) error_exit("Obj file not found!\n");

	while (!fin.eof()) {
		string line;
		getline(fin, line);

		if (!line.size() || line[0] == '#') continue;

		if (line.find("vn") == 0) {
			vertexNormals.push_back(readVec3(line));
		} else if (line.find("v") == 0) {
			vertices.push_back(readVec3(line));
		} else if (line.find("f") == 0) {
			triIndices.push_back(readTri(line));
		}
	}

	fin.close();
}

shared_ptr<Mesh> ObjReader::getMesh() {
	fitSize();
	for (const auto &idx : triIndices) tris.push_back({ vertices[idx[0]], vertices[idx[1]], vertices[idx[2]] });

	if (smoothShadingFlag) {
		if (!vertexNormals.size()) {
			vector<int> vertexTris(vertices.size(), 0);		// vertex has how many adjacent triangles
			vertexNormals = vector<Vec3>(vertices.size(), Vec3::zeros());

			rep(i, tris.size()) {
				Vec3 normal = tris[i].getNormal();
				array<int, 3> idx = triIndices[i];
				rep(k, 3) {
					++vertexTris[idx[k]];
					vertexNormals[idx[k]] += normal;
				}
			}

			rep(i, vertices.size()) vertexNormals[i] /= vertexTris[i];
		}

		for (auto &vertexNormal : vertexNormals) vertexNormal.normalize();
		return make_shared<Mesh>(nullptr, tris, triIndices, vertexNormals);
	} else {
		return make_shared<Mesh>(nullptr, tris, triIndices, vector<Vec3>{});
	}
}

Vec3 ObjReader::readVec3(const string &str) {
	istringstream iss(str);
	string tmp;
	real_t x, y, z;
	iss >> tmp >> x >> y >> z;
	return{ x, y, z };
}

array<int, 3> ObjReader::readTri(const string &str) {
	istringstream iss(str);
	string tmp;
	iss >> tmp;
	
	int cnt = 0;
	for (const auto &ch : str) {
		if (ch == '/') ++cnt;
	}

	if (cnt == 0) {
		int idx[3];
		iss >> idx[0] >> idx[1] >> idx[2];
		array<int, 3> res =  { idx[0] - 1, idx[1] - 1, idx[2] - 1 };
		return res;
	} else if (cnt == 3) {
		error_exit("Not supported type!\n");
	} else if (cnt == 6) {
		int idx[3], tmpInt;
		char tmpCh;
		rep(k, 3) {
			iss >> idx[k] >> tmpCh;
			int nowPos = iss.tellg();
			if (str[nowPos] == tmpCh) iss >> tmpCh;
			else iss >> tmpInt >> tmpCh;
			iss >> tmpInt;
		}
		array<int, 3> res = { idx[0] - 1, idx[1] - 1, idx[2] - 1 };
		return res;
	} else {
		error_exit("Not supported type!\n");
	}
}

void ObjReader::fitSize() {
	moveToCenter();
	scaleToRadius();
}

void ObjReader::moveToCenter() {
	Vec3 meshCenter{ 0.0, 0.0, 0.0 };
	for (const auto &v : vertices) meshCenter += v;
	meshCenter /= vertices.size();

	for (auto &v : vertices) v = v - meshCenter + center;
}

void ObjReader::scaleToRadius() {
	real_t maxDist = 0.0;
	for (const auto &v : vertices) updateMax(maxDist, (v - center).norm());

	for (auto &v : vertices) v = (v - center) * (radius / maxDist) + center;
}