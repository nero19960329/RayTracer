#include "DataGenerator.h"
#include "MonteCarloPathTracing.h"
#include "NeuralNetwork.h"
#include "PredictRayTracing.h"
#include "QuadObj.h"
#include "RayTracing.h"
#include "SceneReader.h"
#include "Sphere.h"
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

	xml_attribute<> *traceNode = root->first_attribute("type");
	if (traceNode) {
		const char *trace_type = traceNode->value();
		if (!strcmp(trace_type, "rt")) traceType = RT;
		else if (!strcmp(trace_type, "mcpt")) traceType = MCPT;
		else if (!strcmp(trace_type, "prt")) traceType = PRT;
	}

	xml_node<> *brdfNode = root->first_node("brdf");
	if (brdfNode) {
		const char *brdf_type = brdfNode->first_attribute("type")->value();
		if (!strcmp(brdf_type, "lambertian")) brdfType = LAMBERTIAN;
		else if (!strcmp(brdf_type, "phong")) brdfType = PHONG;
	}

	for (auto node = root->first_node(); node; node = node->next_sibling()) {
		if (!strcmp(node->name(), "viewer")) readViewer(node);
		else if (!strcmp(node->name(), "light")) readLight(node);
		else if (!strcmp(node->name(), "object")) readObject(node);
	}

	if (tmpObjVec.size()) {
		shared_ptr<KDTree> tree = make_shared<KDTree>(tmpObjVec);
		scene.addObject(tree);
	}

	if (traceType == RT) tracer = make_shared<RayTracing>(scene);
	else if (traceType == MCPT) tracer = make_shared<MonteCarloPathTracing>(scene, brdfType);
	else if (traceType == PRT) {
		if (!root->first_node("nn_path")) error_exit("Doesn't input neural network folder path!\n");

		const char *nn_path = root->first_node("nn_path")->first_attribute("value")->value();
		vector<NeuralNetwork> directNNVec, indirectNNVec;
		rep(i, tmpObjVec.size()) {
			ostringstream oss;
			oss << nn_path << "/direct_" << (i + 1) << ".mat";
			MatReader reader(oss.str());
			directNNVec.push_back(NeuralNetwork(reader));
			oss.str("");
			oss << nn_path << "/indirect_" << (i + 1) << ".mat";
			reader = MatReader(oss.str());
			indirectNNVec.push_back(NeuralNetwork(reader));
		}
		tracer = make_shared<PredictRayTracing>(scene, directNNVec, indirectNNVec);
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
	materialMap["mcpt_frosted_mirror"] = Material::MCPT_FROSTED_MIRROR;
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
		xml_node<> *tmpNode;
		if (*str.rbegin() == '*') tmpNode = parent->first_node(str.substr(0, str.size() - 1).c_str());
		else {
			tmpNode = parent->first_node(str.c_str());
			if (!tmpNode) {
				cout << parent->name() << " node: lack of attribute \"" << str << "\"" << endl;
				error_exit("");
			}
		}
		res.push_back(tmpNode);
	}

	return res;
}

Vec3 SceneReader::readColor(const string &str) const {
	if (str[0] >= '0' && str[0] <= '9') {
		Vec3 res;
		istringstream iss(str);
		iss >> res.x >> res.y >> res.z;
		return res;
	} else {
		auto end = colorMap.cend();
		auto res = colorMap.find(str);
		if (res != end) return res->second;
		else error_exit("No such color!\n");
	}
}

Material SceneReader::readMaterial(const string &str) const {
	if (str[0] >= '0' && str[0] <= '9') {
		istringstream iss(str);
		real_t ambient, specular, shininess, refl, refr;
		iss >> ambient >> specular >> shininess >> refl >> refr;
		return Material{ ambient, specular, shininess, refl, refr };
	} else {
		auto end = materialMap.cend();
		auto res = materialMap.find(str);
		if (res != end) return res->second;
		else error_exit("No such material!\n");
	}
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
			readColor(getValue(textureNodes[1])),
			brdfType
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
		"geo", "center", "target", "up", "fovy", "anti*", "dop*", "view*", "mcpt_sample*"
	});

	viewer = Viewer{
		Geometry{ readPair(viewerNodes[0]) },
		readVec3(viewerNodes[1]),
		readVec3(viewerNodes[2]),
		readVec3(viewerNodes[3]),
		readReal(viewerNodes[4]),
	};

	if (viewerNodes[5]) viewer.setAntiThings(readInt(viewerNodes[5]));
	if (viewerNodes[6]) {
		auto dopNodes = getChildNodes(viewerNodes[6], { "aperture", "offset", "sample" });
		viewer.setDopThings(readReal(dopNodes[0]), readReal(dopNodes[1]), readInt(dopNodes[2]));
	}
	if (viewerNodes[7]) {
		auto viewNodes = getChildNodes(viewerNodes[7], { "size" });
		viewer.setViewPort(Geometry{ readPair(viewNodes[0]) });
	}
	if (viewerNodes[8]) viewer.setMCPTSample(readInt(viewerNodes[8]));
}

void SceneReader::readLight(xml_node<> *node) {
	auto lightNodes = getChildNodes(node, {
		"geo", "color", "intensity", "area*"
	});

	auto geoNodes = getChildNodes(lightNodes[0], {
		"center", "x*", "y*", "length_x*", "length_y*"
	});

	RectGeo rectGeo;
	if (geoNodes[1] && geoNodes[2] && geoNodes[3] && geoNodes[4]) {
		rectGeo = RectGeo{
			readVec3(geoNodes[0]),
			readVec3(geoNodes[1]),
			readVec3(geoNodes[2]),
			readReal(geoNodes[3]),
			readReal(geoNodes[4])
		};
	} else {
		rectGeo = RectGeo{
			readVec3(geoNodes[0]),
			{ 1, 0, 0 },
			{ 0, 1, 0 },
			1e-6,
			1e-6
		};
	}

	shared_ptr<Light> light = make_shared<Light>(Light(
		rectGeo,
		readColor(getValue(lightNodes[1])),
		readReal(lightNodes[2])
		));

	if (!lightNodes[3]) scene.addLight(light);
	else {
		auto areaNodes = getChildNodes(lightNodes[3], {
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
	else if (!strcmp(type, "rect")) readRect(node);
	else if (!strcmp(type, "quad")) readQuad(node);
	else error_exit("No such Object!\n");
}

void SceneReader::readSphere(xml_node<> *node) {
	auto sphereNodes = getChildNodes(node, {
		"texture", "center", "radius", "transparency*"
	});

	bool transFlag = readBool(sphereNodes[3]);
	if (!transFlag) {
		tmpObjVec.push_back(make_shared<Sphere>(
			readTexture(sphereNodes[0]),
			nowObjNum++,
			readVec3(sphereNodes[1]),
			readReal(sphereNodes[2])
			));
	} else {
		auto transNodes = getChildNodes(sphereNodes[3], { "refraction_index" });
		tmpObjVec.push_back(make_shared<Sphere>(
			readTexture(sphereNodes[0]),
			nowObjNum++,
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
		nowObjNum++,
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
		nowObjNum++,
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
	shared_ptr<Mesh> mesh = reader->getMesh(nowObjNum++);
	mesh->setTexture(readTexture(meshNodes[0]));
	tmpObjVec.push_back(mesh);
}

void SceneReader::readRect(xml_node<> *node) {
	auto rectNodes = getChildNodes(node, {
		"texture", "center", "x", "y", "length_x", "length_y"
	});
	tmpObjVec.push_back(make_shared<RectObj>(
		readTexture(rectNodes[0]),
		nowObjNum++,
		readVec3(rectNodes[1]),
		readVec3(rectNodes[2]),
		readVec3(rectNodes[3]),
		readReal(rectNodes[4]),
		readReal(rectNodes[5])
		));
}

void SceneReader::readQuad(xml_node<> *node) {
	auto quadNodes = getChildNodes(node, {
		"texture", "a", "b", "c", "d"
	});
	tmpObjVec.push_back(make_shared<QuadObj>(
		readTexture(quadNodes[0]),
		nowObjNum++,
		readVec3(quadNodes[1]),
		readVec3(quadNodes[2]),
		readVec3(quadNodes[3]),
		readVec3(quadNodes[4])
		));
}