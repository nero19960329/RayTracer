#include <memory>

#include "Constants.h"
#include "Light.h"
#include "Plane.h"
#include "Renderer.h"
#include "Scene.h"
#include "SceneReader.h"
#include "Timer.h"
#include "Viewer.h"

using namespace cv;
using namespace std;

int main() {
	SceneReader sceneReader("../scene/2.scene");
	Renderer renderer{ sceneReader.getViewer(), sceneReader.getScene() };

	Timer timer;
	timer.begin();
	Mat img = renderer.render();
	printf("Duration: %.4lfs\n", timer.getDuration());

	imshow("Ray Tracing", img);
	imwrite("../img/test.png", img);
	waitKey(0);

	return 0;
}