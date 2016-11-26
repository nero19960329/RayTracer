#include "SceneReader.h"
#include "Texture.h"
#include "Utils.h"
#include "Vec3.h"

#include <fstream>

using namespace rapidxml;
using namespace std;

SceneReader::SceneReader(const string &sceneFileName) {
	fillMap();

	file<> fdoc(sceneFileName.c_str());
	if (!fdoc.size()) error_exit("Scene xml file not found!\n");

	xml_document<> doc;
	doc.parse<0>(fdoc.data());

	xml_node<> *root = doc.first_node("scene");
	if (!root) error_exit("Scene xml file format error!\n");

	for (auto node = root->first_node(); node; node = node->next_sibling()) {
		if (!strcmp(node->name(), "viewer")) readViewer(node);
		else if (!strcmp(node->name(), "light")) readLight(node);
		else if (!strcmp(node->name(), "object")) readObject(node);
	}

	if (tmpObjVec.size()) {
		shared_ptr<KDTree> tree = make_shared<KDTree>(tmpObjVec);
		scene.addObject(tree);
	}
}

void SceneReader::fillMap() {
	colorMap["black"] = Vec3::BLACK;
	colorMap["white"] = Vec3::WHITE;
	colorMap["red"] = Vec3::RED;
	colorMap["green"] = Vec3::GREEN;
	colorMap["blue"] = Vec3::BLUE;
	colorMap["yellow"] = Vec3::YELLOW;
	colorMap["cyan"] = Vec3::CYAN;
	colorMap["magenta"] = Vec3::MAGENTA;

	materialMap["plastic"] = Material::PLASTIC;
	materialMap["mirror"] = Material::MIRROR;
	materialMap["a_bit_mirror"] = Material::A_BIT_MIRROR;
	materialMap["transparent_material"] = Material::TRANSPARENT_MATERIAL;
	materialMap["floor"] = Material::FLOOR;

	refrIdxMap["vacuum_refraction_index"] = VACUUM_REFRACTION_INDEX;
	refrIdxMap["water_refraction_index"] = WATER_REFRACTION_INDEX;
	refrIdxMap["glass_refraction_index"] = GLASS_REFRACTION_INDEX;
}

vector<rapidxml::xml_node<> *> SceneReader::getChildNodes(xml_node<> *parent, const vector<string> &strVec) const {
	vector<rapidxml::xml_node<> *> res;

	for (const auto &str : strVec) {
		xml_node<> *tmpNode = parent->first_node(str.c_str());
		if (!tmpNode) error_exit("Lack of some essential attribute!\n");
		res.push_back(tmpNode);
	}

	return res;
}

Vec3 SceneReader::readColor(const string &str) const {
	auto end = colorMap.cend();
	auto res = colorMap.find(str);
	if (res != end) return res->second;
	else error_exit("No such color!\n");
}

Material SceneReader::readMaterial(const string &str) const {
	auto end = materialMap.cend();
	auto res = materialMap.find(str);
	if (res != end) return res->second;
	else error_exit("No such material!\n");
}

real_t SceneReader::readRefrIdx(const string &str) const {
	auto end = refrIdxMap.cend();
	auto res = refrIdxMap.find(str);
	if (res != end) return res->second;
	else error_exit("No such refraction index!\n");
}

shared_ptr<Texture> SceneReader::readTexture(xml_node<> *node) const {
	auto type = node->first_attribute("type")->value();
	if (!strcmp(type, "pure")) {
		auto textureNodes = getChildNodes(node, {
			"material", "color"
		});
		return make_shared<PureTexture>(
			readMaterial(getValue(textureNodes[0])), 
			readColor(getValue(textureNodes[1]))
			);
	} else if (!strcmp(type, "grid")) {
		auto textureNodes = getChildNodes(node, {
			"material", "grid"
		});
		return make_shared<GridTexture>(
			readMaterial(getValue(textureNodes[0])), 
			readInt(textureNodes[1])
			);
	} else if (!strcmp(type, "image")) {
		auto textureNodes = getChildNodes(node, {
			"material", "filename", "length"
		});
		return make_shared<ImageTexture>(
			readMaterial(getValue(textureNodes[0])),
			getValue(textureNodes[1]),
			readInt(textureNodes[2])
			);
	} else error_exit("No such texture!\n");
}

void SceneReader::readViewer(xml_node<> *node) {
	auto viewerNodes = getChildNodes(node, {
		"geo", "center", "target", "up", "fovy", "anti", "dop", "view"
	});

	viewer = Viewer{
		Geometry{ readPair(viewerNodes[0]) },
		readVec3(viewerNodes[1]),
		readVec3(viewerNodes[2]),
		readVec3(viewerNodes[3]),
		readReal(viewerNodes[4]),
	};

	bool antiFlag = readBool(viewerNodes[5]);
	bool dopFlag = readBool(viewerNodes[6]);
	bool viewFlag = readBool(viewerNodes[7]);

	if (antiFlag) {
		auto antiNodes = getChildNodes(viewerNodes[5], { "sample" });
		viewer.setAntiThings(readInt(antiNodes[0]));
	}
	if (dopFlag) {
		auto dopNodes = getChildNodes(viewerNodes[6], { "aperture", "offset", "sample" });
		viewer.setDopThings(readReal(dopNodes[0]), readReal(dopNodes[1]), readInt(dopNodes[2]));
	}
	if (viewFlag) {
		auto viewNodes = getChildNodes(viewerNodes[7], { "size" });
		viewer.setViewPort(Geometry{ readPair(viewNodes[0]) });
	}
}

void SceneReader::readLight(xml_node<> *node) {
	auto lightNodes = getChildNodes(node, {
		"origin", "color", "intensity", "radius", "area"
	});

	shared_ptr<Light> light = make_shared<Light>(Light(
		readVec3(lightNodes[0]),
		readColor(getValue(lightNodes[1])),
		readReal(lightNodes[2]),
		readReal(lightNodes[3])
		));

	bool areaFlag = readBool(lightNodes[4]);
	if (!areaFlag) {
		scene.addLight(light);
	} else {
		auto areaNodes = getChildNodes(lightNodes[4], {
			"sample", "radius"
		});
		int sample = readInt(areaNodes[0]);
		real_t radius = readReal(areaNodes[1]);
		real_t partIntensity = light->intensity / (sample * sample);
		for (int i = 0; i < sample; ++i) {
			real_t theta = 2 * i * PI / sample;
			for (int j = 0; j < sample; ++j) {
				real_t phi = 2 * j * PI / sample;
				shared_ptr<Light> tmpLight = make_shared<Light>();
				*tmpLight = *light;
				tmpLight->setCenter(light->getCenter() + Vec3{ radius * sin(theta) * cos(phi), radius * cos(theta) * cos(phi), radius * sin(phi) });
				tmpLight->intensity = partIntensity;
				scene.addLight(tmpLight);
			}
		}
	}
}

void SceneReader::readObject(xml_node<> *node) {
	auto type = node->first_attribute("type")->value();
	if (!strcmp(type, "sphere")) readSphere(node);
	else if (!strcmp(type, "plane")) readPlane(node);
	else if (!strcmp(type, "face")) readFace(node);
	else if (!strcmp(type, "mesh")) readMesh(node);
	else error_exit("No such Object!\n");
}

void SceneReader::readSphere(xml_node<> *node) {
	auto sphereNodes = getChildNodes(node, {
		"texture", "center", "radius", "transparency"
	});

	bool transFlag = readBool(sphereNodes[3]);
	if (!transFlag) {
		tmpObjVec.push_back(make_shared<Sphere>(
			readTexture(sphereNodes[0]),
			readVec3(sphereNodes[1]),
			readReal(sphereNodes[2])
			));
	} else {
		auto transNodes = getChildNodes(sphereNodes[3], { "refraction_index" });
		tmpObjVec.push_back(make_shared<Sphere>(
			readTexture(sphereNodes[0]),
			readVec3(sphereNodes[1]),
			readReal(sphereNodes[2]),
			readRefrIdx(getValue(transNodes[0]))
			));
	}
}

void SceneReader::readPlane(xml_node<> *node) {
	auto planeNodes = getChildNodes(node, {
		"texture", "normal", "offset"
	});
	scene.addObject(make_shared<Plane>(
		readTexture(planeNodes[0]),
		readVec3(planeNodes[1]),
		readReal(planeNodes[2]),
		false
		));
}

void SceneReader::readFace(xml_node<> *node) {
	auto faceNodes = getChildNodes(node, {
		"texture", "a", "b", "c"
	});
	tmpObjVec.push_back(make_shared<Face>(
		readTexture(faceNodes[0]),
		readVec3(faceNodes[1]),
		readVec3(faceNodes[2]),
		readVec3(faceNodes[3])
		));
}

void SceneReader::readMesh(xml_node<> *node) {
	auto meshNodes = getChildNodes(node, {
		"texture", "filename", "center", "radius", "smooth"
	});
	shared_ptr<ObjReader> reader = make_shared<ObjReader>(
		getValue(meshNodes[1]),
		readVec3(meshNodes[2]),
		readReal(meshNodes[3]),
		readBool(meshNodes[4])
		);
	shared_ptr<Mesh> mesh = reader->getMesh();
	mesh->setTexture(readTexture(meshNodes[0]));
	tmpObjVec.push_back(mesh);
}