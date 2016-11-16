#pragma once

#include "Face.h"
#include "InfPlane.h"
#include "Light.h"
#include "Mesh.h"
#include "Object.h"
#include "ObjReader.h"
#include "Plane.h"
#include "PureSphere.h"
#include "Scene.h"
#include "Tri.h"
#include "Utils.h"
#include "Viewer.h"

#include <functional>
#include <memory>
#include <unordered_map>

class SceneReader {
private:
	Scene scene;
	Viewer viewer;

	std::vector<std::shared_ptr<Object>> tmpObjVec;

	std::unordered_map<std::string, Vec3> colorMap;
	std::unordered_map<std::string, Material> materialMap;
	std::unordered_map<std::string, real_t> refrIdxMap;

	struct ViewerInitializer {
		Geometry screen{ 0, 0 }, viewport{ 0, 0 };
		Vec3 center, target, up;
		real_t fovy;

		bool dopFlag = false, antiFlag = false;
		real_t apertureSize = 0.1, focusOffset = 0.0;
		int dopSample = 1, antiSample = 1;
	} viewerInitializer;

public:
	explicit SceneReader(const std::string &sceneFileName);
	~SceneReader() {}

	const Scene &getScene() const {
		return scene;
	}

	const Viewer &getViewer() const {
		return viewer;
	}

private:
	int readSth(std::ifstream &fin, std::function<bool(std::istringstream &, const std::string &)> readFunc, std::function<void()> modifyFunc);

	int readViewer(std::ifstream &fin);
	int readLight(std::ifstream &fin);
	int readSphere(std::ifstream &fin);
	int readPlane(std::ifstream &fin);
	int readFace(std::ifstream &fin);
	int readMesh(std::ifstream &fin);

	void fillMap();

	Vec3 readColor(const std::string &str) const;
	Material readMaterial(const std::string &str) const;
	real_t readRefrIdx(const std::string &str) const;

	std::shared_ptr<Texture> readTexture(const std::string &str) const;
};