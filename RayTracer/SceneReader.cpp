#include "SceneReader.h"
#include "Texture.h"
#include "Vec3.h"

#include <fstream>

using namespace std;

SceneReader::SceneReader(const string &sceneFileName) {
	fillMap();

	ifstream fin(sceneFileName);
	while (!fin.eof()) {
		string line;
		getline(fin, line);

		if (!line.size() || line[0] == '#') continue;

		int backTrack = 1;
		if (line.find("Viewer") == 0) {
			backTrack = readViewer(fin);
		} else if (line.find("Light") == 0) {
			shared_ptr<Light> nowLight = make_shared<Light>();
			backTrack = readLight(fin, nowLight);
		} else if (line.find("Sphere") == 0) {
			shared_ptr<Sphere> nowSphere = make_shared<Sphere>();
			backTrack = readSphere(fin, nowSphere);
		} else if (line.find("Plane") == 0) {
			shared_ptr<Plane> nowPlane = make_shared<Plane>();
			backTrack = readPlane(fin, nowPlane);
		} else if (line.find("Tri") == 0) {
			shared_ptr<Tri> nowTri = make_shared<Tri>();
			backTrack = readTri(fin, nowTri);
		}

		if (backTrack <= 0) fin.seekg(backTrack, ios::cur);
		else break;
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
		return std::make_shared<PureTexture>(readMaterial(materialStr), readColor(colorStr));
	} else if (catagory == "GridTexture") {
		int grid;
		iss >> grid;
		return std::make_shared<GridTexture>(readMaterial(materialStr), grid);
	} else if (catagory == "ImageTexture") {
		string fileName;
		iss >> fileName;
		return std::make_shared<ImageTexture>(readMaterial(materialStr), fileName);
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
			Vec3 center;
			iss >> center[0] >> center[1] >> center[2];
			viewerInitializer.center = center;
		} else if (line.find("target") == 0) {
			Vec3 target;
			iss >> target[0] >> target[1] >> target[2];
			viewerInitializer.target = target;
		} else if (line.find("up") == 0) {
			Vec3 up;
			iss >> up[0] >> up[1] >> up[2];
			viewerInitializer.up = up;
		} else if (line.find("fovy") == 0) {
			real_t fovy;
			iss >> fovy;
			viewerInitializer.fovy = fovy;
		} else {
			return false;
		}
		return true;
	}, [&]() {
		viewer = Viewer{ viewerInitializer.screen, viewerInitializer.center, viewerInitializer.target, viewerInitializer.up, viewerInitializer.fovy };
	});
}

int SceneReader::readLight(ifstream &fin, shared_ptr<Light> nowLight) {
	return readSth(fin, [&](istringstream &iss, const string &line) {
		if (line.find("origin") == 0) {
			iss >> nowLight->center[0] >> nowLight->center[1] >> nowLight->center[2];
		} else if (line.find("color") == 0) {
			string color;
			iss >> color;
			nowLight->color = readColor(color);
		} else if (line.find("intensity") == 0) {
			iss >> nowLight->intensity;
		} else {
			return false;
		}
		return true;
	}, [&]() {
		scene.addLight(nowLight);
	});
}

int SceneReader::readSphere(ifstream &fin, shared_ptr<Sphere> nowSphere) {
	return readSth(fin, [&](istringstream &iss, const string &line) {
		if (line.find("texture") == 0) {
			nowSphere->setTexture(readTexture(line.substr(iss.tellg())));
		} else if (line.find("center") == 0) {
			Vec3 center;
			iss >> center[0] >> center[1] >> center[2];
			nowSphere->center = center;
		} else if (line.find("radius") == 0) {
			iss >> nowSphere->radius;
		} else if (line.find("refraction_index") == 0) {
			string refrIdxStr;
			iss >> refrIdxStr;
			nowSphere->innerRefrIdx = readRefrIdx(refrIdxStr);
		} else {
			return false;
		}
		return true;
	}, [&]() {
		scene.addObject(nowSphere);
	});
}

int SceneReader::readPlane(ifstream &fin, shared_ptr<Plane> nowPlane) {
	return readSth(fin, [&](istringstream &iss, const string &line) {
		if (line.find("texture") == 0) nowPlane->setTexture(readTexture(line.substr(iss.tellg())));
		else if (line.find("normal") == 0) iss >> nowPlane->normal[0] >> nowPlane->normal[1] >> nowPlane->normal[2];
		else if (line.find("offset") == 0) iss >> nowPlane->offset;
		else return false;
		return true;
	}, [&]() {
		nowPlane->normal.normalize();
		scene.addObject(nowPlane);
	});
}

int SceneReader::readTri(ifstream &fin, shared_ptr<Tri> nowTri) {
	return readSth(fin, [&](istringstream &iss, const string &line) {
		if (line.find("texture") == 0) nowTri->setTexture(readTexture(line.substr(iss.tellg())));
		else if (line.find("a") == 0) iss >> nowTri->a[0] >> nowTri->a[1] >> nowTri->a[2];
		else if (line.find("b") == 0) iss >> nowTri->b[0] >> nowTri->b[1] >> nowTri->b[2];
		else if (line.find("c") == 0) iss >> nowTri->c[0] >> nowTri->c[1] >> nowTri->c[2];
		else return false;
		return true;
	}, [&]() {
		scene.addObject(nowTri);
	});
}