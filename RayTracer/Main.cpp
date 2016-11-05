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
	scene.addObject(make_shared<Sphere>(Sphere(make_shared<GridTexture>(16), { -3.0, 1.0, -10.0 }, 2.0)));
	//scene.addObject(make_shared<Sphere>(Sphere(make_shared<PureTexture>(Surface::BLUE), { 3.0, 1.0, -10.0 }, 2.0)));
	scene.addObject(make_shared<Sphere>(Sphere(make_shared<ImageTexture>("../texture/stone.jpg"), { 3.0, 1.0, -10.0 }, 2.0)));
	scene.addObject(make_shared<Plane>(Plane(make_shared<ImageTexture>("../texture/floor.jpg"), { 0.0, 1.0, 0.0 }, -1.0)));
	//scene.addObject(make_shared<Plane>(Plane(make_shared<PureTexture>(Surface::WHITE), { 0.0, 1.0, 0.0 }, -2.0)));
	Renderer renderer{ viewer, scene };

	Timer timer;
	timer.begin();
	Mat img = renderer.render();
	printf("Duration: %.4lfs\n", timer.getDuring());

	imshow("Ray Tracing", img);
	imwrite("../img/test.png", img);
	waitKey(0);

	return 0;
}