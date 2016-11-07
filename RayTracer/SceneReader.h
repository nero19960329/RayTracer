#pragma once

#include "Light.h"
#include "Object.h"
#include "Plane.h"
#include "Scene.h"
#include "Triangle.h"
#include "Utils.h"
#include "Viewer.h"

#include <functional>
#include <memory>
#include <unordered_map>

class SceneReader {
private:
	Scene scene;
	Viewer viewer;

	std::unordered_map<std::string, Vec3> colorMap;
	std::unordered_map<std::string, Material> materialMap;
	std::unordered_map<std::string, real_t> refrIdxMap;

	struct ViewerInitializer {
		Geometry screen{ 0, 0 };
		Vec3 center, target, up;
		real_t fovy;
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
	int readLight(std::ifstream &fin, std::shared_ptr<Light>);
	int readSphere(std::ifstream &fin, std::shared_ptr<Sphere>);
	int readPlane(std::ifstream &fin, std::shared_ptr<Plane>);
	int readTri(std::ifstream &fin, std::shared_ptr<Tri>);

	void fillMap();

	Vec3 readColor(const std::string &str) const;
	Material readMaterial(const std::string &str) const;
	real_t readRefrIdx(const std::string &str) const;

	std::shared_ptr<Texture> readTexture(const std::string &str) const;
};