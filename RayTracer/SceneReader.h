#pragma once

#include "BRDF.h"
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

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

class Renderer;

class SceneReader {
	friend class Renderer;

protected:
	Scene scene;
	Viewer viewer;

	std::vector<std::shared_ptr<Object>> tmpObjVec;

	std::unordered_map<std::string, Vec3> colorMap;
	std::unordered_map<std::string, Material> materialMap;
	std::unordered_map<std::string, real_t> refrIdxMap;

	BRDFType brdfType = LAMBERTIAN;
	TraceType traceType = RT;

public:
	explicit SceneReader(const std::string &sceneFileName);
	~SceneReader() {}

	const Scene &getScene() const { return scene; }

	const Viewer &getViewer() const { return viewer; }

	const TraceType &getTraceType() const { return traceType; }

	const BRDFType &getBRDFType() const { return brdfType; }

private:
	std::vector<rapidxml::xml_node<> *> getChildNodes(rapidxml::xml_node<> *parent, const std::vector<std::string> &strVec) const;

	std::string getValue(rapidxml::xml_node<> *node) const { return node->first_attribute("value")->value(); }

	std::pair<int, int> readPair(rapidxml::xml_node<> *node) const {
		std::string str = getValue(node);
		std::istringstream iss(str);
		std::pair<int, int> res;
		iss >> res.first >> res.second;
		return res;
	}

	Vec3 readVec3(rapidxml::xml_node<> *node) const {
		std::string str = getValue(node);
		std::istringstream iss(str);
		Vec3 res;
		iss >> res.x >> res.y >> res.z;
		return res;
	}

	real_t readReal(rapidxml::xml_node<> *node) const {
		std::string str = getValue(node);
		std::istringstream iss(str);
		real_t res;
		iss >> res;
		return res;
	}

	bool readBool(rapidxml::xml_node<> *node) const {
		std::string str = getValue(node);
		std::istringstream iss(str);
		std::string tmp;
		iss >> tmp;
		if (tmp == "true" || tmp == "1") return true;
		else return false;
	}

	int readInt(rapidxml::xml_node<> *node) const {
		std::string str = getValue(node);
		std::istringstream iss(str);
		int res;
		iss >> res;
		return res;
	}

	void readViewer(rapidxml::xml_node<> *node);
	void readLight(rapidxml::xml_node<> *node);
	void readObject(rapidxml::xml_node<> *node);
	void readSphere(rapidxml::xml_node<> *node);
	void readPlane(rapidxml::xml_node<> *node);
	void readFace(rapidxml::xml_node<> *node);
	void readMesh(rapidxml::xml_node<> *node);
	void readRect(rapidxml::xml_node<> *node);
	void readQuad(rapidxml::xml_node<> *node);

	void fillMap();

	Vec3 readColor(const std::string &str) const;
	Material readMaterial(const std::string &str) const;
	real_t readRefrIdx(const std::string &str) const;

	std::shared_ptr<Texture> readTexture(rapidxml::xml_node<> *node) const;
};