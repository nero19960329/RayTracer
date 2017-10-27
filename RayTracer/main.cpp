#include "camera.h"
#include "face.h"
#include "material.h"
#include "mcpt.h"
#include "mesh.h"
#include "plane.h"
#include "scene.h"
#include "sphere.h"
#include "texture.h"
#include "ray_tracing.h"
#include "renderer.h"

#include <iostream>

Material * floorMaterial = new Material(0.0, 0.0);
Material * plasticMaterial = new Material;
Material * transparentMaterial = new Material(0.0, 0.0, 0, 0.0, 1.0);

PureTexture * redTexture = new PureTexture(*plasticMaterial, glm::dvec3{ 1.0, 0.0, 0.0 });
PureTexture * greenTexture = new PureTexture(*plasticMaterial, glm::dvec3{ 0.0, 1.0, 0.0 });
PureTexture * blueTexture = new PureTexture(*plasticMaterial, glm::dvec3{ 0.0, 0.0, 1.0 });
PureTexture * whiteTexture = new PureTexture(*plasticMaterial, glm::dvec3{ 1.0, 1.0, 1.0 });
PureTexture * transparentTexture = new PureTexture(*transparentMaterial, glm::dvec3{ 0.0, 0.0, 0.0 });
GridTexture * gridTexture = new GridTexture(*floorMaterial);

cv::Mat testScene1() {
	Mesh * mesh = new Mesh(
		redTexture,
		"../objs/dragon.obj"
	);
	mesh->scaleToBoundingSphere(glm::dvec3(1.0, 0.0, 0.0), 1.0);
	mesh->update();

	Sphere * sphere1 = new Sphere(
		greenTexture,
		glm::dvec3{ -1.0, 0.0, 2.0 },
		0.5
	);

	Sphere * sphere2 = new Sphere(
		redTexture,
		glm::dvec3{ 1.0, 0.0, 0.0 },
		0.5
	);

	Plane * plane = new Plane(
		new ImageTexture(*floorMaterial, "../texture/floor.jpg"),
		glm::dvec3{ 0.0, 1.0, 0.0 },
		glm::dvec3{ 0.0, -0.5, 0.0 }
	);

	glm::dvec3 center{ 0.0, 0.0, 5.0 };
	glm::dvec3 target{ 0.0, 0.0, 0.0 };
	glm::dvec3 up{ 0.0, 1.0, 0.0 };

	PointLight * light1 = new PointLight(glm::dvec3{ 0.0, 5.0, 3.0 }, glm::dvec3{ 1.0, 1.0, 1.0 }, 2.0);

	Scene scene;
	scene.objs.push_back(mesh);
	scene.objs.push_back(sphere1);
	//scene.objs.push_back(sphere2);
	scene.objs.push_back(plane);
	scene.lights.push_back(light1);

	Camera camera(
		800, 600,
		center,
		target,
		up,
		45.0
	);

	//RayTracing * tracer = new RayTracing(scene);
	MonteCarloPathTracing * tracer = new MonteCarloPathTracing(scene, PHONG);
	RaySampler sampler(camera);
	sampler.mcptMode = true;
	sampler.mcptSampleNum = 10;
	//sampler.rtMode = true;
	//sampler.jitterMode = true;
	//sampler.jitterSampleNum = 5;

	Renderer renderer(camera, tracer, sampler);
	return renderer.render();
}

cv::Mat testScene2() {	// Cornell Box
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, "../objs/CornellBox-Original.obj");
	if (!err.empty())
		std::cerr << err << std::endl;
	if (!ret)
		exit(0);
	Scene scene;
	for (int s = 0; s < shapes.size(); ++s) {
		Mesh * mesh = new Mesh(shapes[s], materials[shapes[s].mesh.material_ids[0]], attrib);
		mesh->update();
		scene.objs.emplace_back(mesh);
	}

	RectLight * light1 = new RectLight(
		glm::dvec3(-0.005, 1.98, -0.03),
		glm::dvec3(0.47, 0.0, 0.0),
		glm::dvec3(0.0, 0.0, 0.38),
		glm::dvec3(0.0, -1.0, 0.0),
		glm::dvec3(1.0, 1.0, 1.0),
		25.0
	);

	RectLight * light2 = new RectLight(
		glm::dvec3(0.98, 0.795, -0.025),
		glm::dvec3(0.0, 0.2, 0.0),
		glm::dvec3(0.0, 0.0, 0.2),
		glm::dvec3(-1.0, 0.0, 0.0),
		glm::dvec3(1.0, 1.0, 1.0),
		10.0
	);

	scene.lights.emplace_back(light1);
	//scene.lights.emplace_back(light2);
	scene.computeLightCDF();

	glm::dvec3 center{ 0.0, 1.0, 3.4 };
	glm::dvec3 target{ 0.0, 1.0, 0.0 };
	glm::dvec3 up{ 0.0, 1.0, 0.0 };

	Camera camera(
		600, 600,
		center,
		target,
		up,
		45.0
	);

	//RayTracing * tracer = new RayTracing(scene);
	MonteCarloPathTracing * tracer = new MonteCarloPathTracing(scene, PHONG);
	RaySampler sampler(camera);
	sampler.mcptMode = true;
	sampler.mcptSampleNum = 10;
	//sampler.rtMode = true;
	//sampler.jitterMode = true;
	//sampler.jitterSampleNum = 5;

	Renderer renderer(camera, tracer, sampler);
	return renderer.render();
}

cv::Mat testScene3() {	// Cornell Box With Sphere
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, "../objs/CornellBox-Sphere.obj");
	if (!err.empty())
		std::cerr << err << std::endl;
	if (!ret)
		exit(0);
	Scene scene;
	for (int s = 2; s < shapes.size(); ++s) {
		Mesh * mesh = new Mesh(shapes[s], materials[shapes[s].mesh.material_ids[0]], attrib);
		mesh->update();
		scene.objs.emplace_back(mesh);
	}

	Sphere * leftSphere = new Sphere(
		new PureTexture(Material(0.01, 0.95, 1000.0), one_vec3 * 0.01),
		//new PureTexture(*floorMaterial, glm::dvec3(1.0, 1.0, 1.0)),
		glm::dvec3(-0.4214, 0.3321, -0.28),
		0.3263
	);
	Sphere * rightSphere = new Sphere(
		new PureTexture(Material(0.01, 0.01, 200.0, 0.0, 0.95), one_vec3 * 0.01),
		//new PureTexture(*floorMaterial, glm::dvec3(1.0, 1.0, 1.0)),
		glm::dvec3(0.4458, 0.3321, 0.3768),
		0.3263,
		1.5
	);
	scene.objs.push_back(leftSphere);
	scene.objs.push_back(rightSphere);

	RectLight * light = new RectLight(
		glm::dvec3(-0.005, 1.58, -0.03),
		glm::dvec3(0.47, 0.0, 0.0),
		glm::dvec3(0.0, 0.0, 0.38),
		glm::dvec3(0.0, -1.0, 0.0),
		glm::dvec3(1.0, 1.0, 1.0),
		30.0
	);

	scene.lights.emplace_back(light);
	scene.computeLightCDF();

	glm::dvec3 center{ 0.0, 0.8, 2.9 };
	glm::dvec3 target{ 0.0, 0.8, 0.0 };
	glm::dvec3 up{ 0.0, 1.0, 0.0 };

	Camera camera(
		760, 600,
		center,
		target,
		up,
		45.0
	);

	//RayTracing * tracer = new RayTracing(scene);
	MonteCarloPathTracing * tracer = new MonteCarloPathTracing(scene, PHONG);
	RaySampler sampler(camera);
	sampler.mcptMode = true;
	sampler.mcptSampleNum = 10;
	//sampler.rtMode = true;
	//sampler.jitterMode = true;
	//sampler.jitterSampleNum = 5;

	Renderer renderer(camera, tracer, sampler);
	return renderer.render();
}

int main() {
	cv::Mat img = testScene3();
	cv::imshow("result", img);
	cv::imwrite("tmp.png", img);
	cv::waitKey();

	return 0;
}