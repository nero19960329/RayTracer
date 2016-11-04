#include <memory>

#include "Constants.h"
#include "Light.h"
#include "Plane.h"
#include "Renderer.h"
#include "Scene.h"
#include "Viewer.h"

using namespace cv;
using namespace std;

int main() {
	Viewer viewer{ { 800, 600 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, -1.0 }, { 0.0, 1.0, 0.0 }, 60 };
	Scene scene;
	scene.addLight(make_shared<Light>(Light({ 0.0, 0.0, 1.0 }, { 0.25, 0.25, 0.25 })));
	scene.addLight(make_shared<Light>(Light({ -1.0, 5.0, -10.0 }, { 1.0, 1.0, 1.0 })));
	scene.addObject(make_shared<Sphere>(Sphere(make_shared<PureTexture>(Surface::MAGENTA), { -3.0, 1.0, -10.0 }, 2.0)));
	scene.addObject(make_shared<Sphere>(Sphere(make_shared<PureTexture>(Surface::BLUE), { 3.0, 1.0, -10.0 }, 2.0)));
	scene.addObject(make_shared<Plane>(Plane(make_shared <PureTexture>(Surface::WHITE), { 0.0, 1.0, 0.0 }, -1.0)));
	Renderer renderer{ viewer, scene };
	Mat img = renderer.render();

	imshow("Ray Tracing", img);
	imwrite("../img/test.png", img);
	waitKey(0);

	return 0;
}