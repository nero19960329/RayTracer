#include "SceneReader.h"
#include "Texture.h"
#include "Utils.h"
#include "Vec3.h"

#include <fstream>

using namespace std;

SceneReader::SceneReader(const string &sceneFileName) {
	fillMap();

	ifstream fin(sceneFileName);
	if (!fin) error_exit("Scene file not found!\n");

	while (!fin.eof()) {
		string line;
		getline(fin, line);

		if (!line.size() || line[0] == '#') continue;

		int backTrack = 1;
		shared_ptr<Texture> texture = nullptr;
		if (line.find("Viewer") == 0) backTrack = readViewer(fin);
		else if (line.find("Light") == 0) backTrack = readLight(fin);
		else if (line.find("Sphere") == 0) backTrack = readSphere(fin);
		else if (line.find("Plane") == 0) backTrack = readPlane(fin);
		else if (line.find("Face") == 0) backTrack = readFace(fin);
		else if (line.find("Mesh") == 0) backTrack = readMesh(fin);

		if (backTrack <= 0) fin.seekg(backTrack, ios::cur);
		else break;
	}

	if (tmpObjVec.size()) {
		shared_ptr<KDTree> tree = make_shared<KDTree>(tmpObjVec);
		scene.addObject(tree);
	}
}

void SceneReader::fillMap() {
	colorMap["BLACK"] = Vec3::BLACK;
	colorMap["WHITE"] = Vec3::WHITE;
	colorMap["RED"] = Vec3::RED;
	colorMap["GREEN"] = Vec3::GREEN;
	colorMap["BLUE"] = Vec3::BLUE;
	colorMap["YELLOW"] = Vec3::YELLOW;
	colorMap["CYAN"] = Vec3::CYAN;
	colorMap["MAGENTA"] = Vec3::MAGENTA;

	materialMap["PLASTIC"] = Material::PLASTIC;
	materialMap["MIRROR"] = Material::MIRROR;
	materialMap["A_BIT_MIRROR"] = Material::A_BIT_MIRROR;
	materialMap["TRANSPARENT_MATERIAL"] = Material::TRANSPARENT_MATERIAL;
	materialMap["FLOOR"] = Material::FLOOR;

	refrIdxMap["VACUUM_REFRACTION_INDEX"] = VACUUM_REFRACTION_INDEX;
	refrIdxMap["WATER_REFRACTION_INDEX"] = WATER_REFRACTION_INDEX;
	refrIdxMap["GLASS_REFRACTION_INDEX"] = GLASS_REFRACTION_INDEX;
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

shared_ptr<Texture> SceneReader::readTexture(const string &str) const {
	istringstream iss(str);
	string catagory, materialStr;
	iss >> catagory >> materialStr;

	if (catagory == "PureTexture") {
		string colorStr;
		iss >> colorStr;
		return make_shared<PureTexture>(readMaterial(materialStr), readColor(colorStr));
	} else if (catagory == "GridTexture") {
		int grid;
		iss >> grid;
		return make_shared<GridTexture>(readMaterial(materialStr), grid);
	} else if (catagory == "ImageTexture") {
		string fileName;
		int length;
		iss >> fileName;
		if (!(iss >> length)) return make_shared<ImageTexture>(readMaterial(materialStr), fileName);
		else return make_shared<ImageTexture>(readMaterial(materialStr), fileName, length);
	} else {
		error_exit("No such texture!\n");
	}
}

int SceneReader::readSth(ifstream &fin, function<bool(istringstream &, const string &)> readFunc, function<void()> modifyFunc) {
	auto place = fin.tellg();

	bool isEOF = true;
	while (!fin.eof()) {
		string line;
		getline(fin, line);

		if (!line.size() || line[0] == '#') {
			place = fin.tellg();
			continue;
		}

		istringstream iss(line);
		string tmp;
		iss >> tmp;
		if (!readFunc(iss, line)) {
			isEOF = false;
			break;
		}

		place = fin.tellg();
	}

	modifyFunc();

	if (!isEOF) return place - fin.tellg();
	else return 1;
}

int SceneReader::readViewer(ifstream &fin) {
	return readSth(fin, [&](istringstream &iss, const string &line) {
		if (line.find("geo") == 0) {
			int w, h;
			iss >> w >> h;
			viewerInitializer.screen = Geometry(w, h);
		} else if (line.find("center") == 0) {
			iss >> viewerInitializer.center[0] >> viewerInitializer.center[1] >> viewerInitializer.center[2];
		} else if (line.find("target") == 0) {
			iss >> viewerInitializer.target[0] >> viewerInitializer.target[1] >> viewerInitializer.target[2];
		} else if (line.find("up") == 0) {
			iss >> viewerInitializer.up[0] >> viewerInitializer.up[1] >> viewerInitializer.up[2];
		} else if (line.find("fovy") == 0) {
			iss >> viewerInitializer.fovy;
		} else if (line.find("dop") == 0) {
			viewerInitializer.dopFlag = true;
			iss >> viewerInitializer.apertureSize >> viewerInitializer.focusOffset >> viewerInitializer.dopSample;
		} else if (line.find("anti") == 0) {
			viewerInitializer.antiFlag = true;
			iss >> viewerInitializer.antiSample;
		} else {
			return false;
		}
		return true;
	}, [&]() {
		viewer = Viewer{ viewerInitializer.screen, viewerInitializer.center, viewerInitializer.target, viewerInitializer.up, viewerInitializer.fovy };
		if (viewerInitializer.dopFlag) {
			viewer.dopFlag = true;
			viewer.apertureSize = viewerInitializer.apertureSize;
			viewer.focusOffset = viewerInitializer.focusOffset;
			viewer.dopSample = viewerInitializer.dopSample;
		}
		if (viewerInitializer.antiFlag) {
			viewer.antialiasingFlag = true;
			viewer.antiSample = viewerInitializer.antiSample;
		}
	});
}

int SceneReader::readLight(ifstream &fin) {
	shared_ptr<Light> nowLight = make_shared<Light>();
	bool areaFlag = false;
	int sample;
	real_t radius;
	return readSth(fin, [&](istringstream &iss, const string &line) {
		if (line.find("origin") == 0) {
			Vec3 center;
			iss >> center[0] >> center[1] >> center[2];
			nowLight->setCenter(center);
		} else if (line.find("color") == 0) {
			string color;
			iss >> color;
			nowLight->color = readColor(color);
		} else if (line.find("intensity") == 0) {
			iss >> nowLight->intensity;
		} else if (line.find("area") == 0) {
			areaFlag = true;
			iss >> sample >> radius;
		} else {
			return false;
		}
		return true;
	}, [&]() {
		if (areaFlag) {
			real_t partIntensity = nowLight->intensity / (sample * sample);
			for (int i = 0; i < sample; ++i) {
				real_t theta = 2 * i * PI / sample;
				for (int j = 0; j < sample; ++j) {
					real_t phi = 2 * j * PI / sample;
					shared_ptr<Light> tmpLight = make_shared<Light>();
					*tmpLight = *nowLight;
					tmpLight->setCenter(nowLight->getCenter() + Vec3{ radius * sin(theta) * cos(phi), radius * cos(theta) * cos(phi), radius * sin(phi) });
					tmpLight->intensity = partIntensity;
					scene.addLight(tmpLight);
				}
			}
		} else {
			scene.addLight(nowLight);
		}
	});
}

int SceneReader::readSphere(ifstream &fin) {
	PureSphere nowPureSphere;
	real_t refrIdx = VACUUM_REFRACTION_INDEX;
	shared_ptr<Texture> texture;
	return readSth(fin, [&](istringstream &iss, const string &line) {
		if (line.find("texture") == 0) {
			texture = readTexture(line.substr(iss.tellg()));
		} else if (line.find("center") == 0) {
			iss >> nowPureSphere.center[0] >> nowPureSphere.center[1] >> nowPureSphere.center[2];
		} else if (line.find("radius") == 0) {
			iss >> nowPureSphere.radius;
		} else if (line.find("refraction_index") == 0) {
			string refrIdxStr;
			iss >> refrIdxStr;
			refrIdx = readRefrIdx(refrIdxStr);
		} else {
			return false;
		}
		return true;
	}, [&]() {
		tmpObjVec.push_back(make_shared<Sphere>(texture, nowPureSphere, refrIdx));
	});
}

int SceneReader::readPlane(ifstream &fin) {
	InfPlane nowInfPlane;
	shared_ptr<Texture> texture;
	return readSth(fin, [&](istringstream &iss, const string &line) {
		if (line.find("texture") == 0) texture = readTexture(line.substr(iss.tellg()));
		else if (line.find("normal") == 0) iss >> nowInfPlane.normal[0] >> nowInfPlane.normal[1] >> nowInfPlane.normal[2];
		else if (line.find("offset") == 0) iss >> nowInfPlane.offset;
		else return false;
		return true;
	}, [&]() {
		scene.addObject(make_shared<Plane>(texture, nowInfPlane.normal, nowInfPlane.offset, false));
	});
}

int SceneReader::readFace(ifstream &fin) {
	Tri nowTri;
	shared_ptr<Texture> texture;
	return readSth(fin, [&](istringstream &iss, const string &line) {
		if (line.find("texture") == 0) texture = readTexture(line.substr(iss.tellg()));
		else if (line.find("a") == 0) iss >> nowTri.a[0] >> nowTri.a[1] >> nowTri.a[2];
		else if (line.find("b") == 0) iss >> nowTri.b[0] >> nowTri.b[1] >> nowTri.b[2];
		else if (line.find("c") == 0) iss >> nowTri.c[0] >> nowTri.c[1] >> nowTri.c[2];
		else return false;
		return true;
	}, [&]() {
		tmpObjVec.push_back(make_shared<Face>(texture, nowTri));
	});
}

int SceneReader::readMesh(ifstream &fin) {
	shared_ptr<ObjReader> nowReader;
	shared_ptr<Texture> texture;
	return readSth(fin, [&](istringstream &iss, const string &line) {
		if (line.find("texture") == 0) {
			texture = readTexture(line.substr(iss.tellg()));
		} else if (line.find("file") == 0) {
			string fileName;
			iss >> fileName;
			nowReader = make_shared<ObjReader>(fileName);
		} else if (line.find("center") == 0) {
			Vec3 center;
			iss >> center[0] >> center[1] >> center[2];
			if (!nowReader) error_exit("Mesh: must input \"file\" first!\n");
			nowReader->setCenter(center);
		} else if (line.find("radius") == 0) {
			real_t radius;
			iss >> radius;
			if (!nowReader) error_exit("Mesh: must input \"file\" first!\n");
			nowReader->setRadius(radius);
		} else if (line.find("smooth") == 0) {
			string flag;
			iss >> flag;
			if (flag == "true" || flag == "True") nowReader->setSmoothShading(true);
			else nowReader->setSmoothShading(false);
		} else {
			return false;
		}
		return true;
	}, [&]() {
		shared_ptr<Mesh> mesh = nowReader->getMesh();
		mesh->setTexture(texture);
		tmpObjVec.push_back(mesh);
	});
}