#include <memory>

#include "Constants.h"
#include "Light.h"
#include "Plane.h"
#include "Renderer.h"
#include "Scene.h"
#include "Timer.h"
#include "Viewer.h"

using namespace cv;
using namespace std;

int main() {
	Viewer viewer{ { 800, 600 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, -1.0 }, { 0.0, 1.0, 0.0 }, 60 };
	Scene scene;
	scene.addLight(make_shared<Light>(Light({ 0.0, 0.0, 1.0 }, { 1.0, 1.0, 1.0 }, 0.25)));
	scene.addLight(make_shared<Light>(Light({ -1.0, 5.0, -10.0 }, { 1.0, 1.0, 1.0 }, 1.0)));
	scene.addObject(make_shared<Sphere>(Sphere(make_shared<GridTexture>(Material::A_BIT_MIRROR, 16), { -3.0, 1.0, -10.0 }, 2.0)));
	//scene.addObject(make_shared<Sphere>(Sphere(make_shared<PureTexture>(Material::A_BIT_MIRROR, Vec3::BLUE), { -3.0, 1.0, -10.0 }, 2.0)));
	scene.addObject(make_shared<Sphere>(Sphere(make_shared<ImageTexture>(Material::PLASTIC, "../texture/stone.jpg"), { 3.0, 1.0, -10.0 }, 2.0)));
	scene.addObject(make_shared<Plane>(Plane(make_shared<ImageTexture>(Material::FLOOR, "../texture/floor.jpg"), { 0.0, 1.0, 0.0 }, -1.0)));
	//scene.addObject(make_shared<Plane>(Plane(make_shared<PureTexture>(Material::FLOOR, Vec3::WHITE), { 0.0, 1.0, 0.0 }, -2.0)));
	Renderer renderer{ viewer, scene };

	Timer timer;
	timer.begin();
	Mat img = renderer.render();
	printf("Duration: %.4lfs\n", timer.getDuration());

	imshow("Ray Tracing", img);
	imwrite("../img/test.png", img);
	waitKey(0);

	return 0;
}