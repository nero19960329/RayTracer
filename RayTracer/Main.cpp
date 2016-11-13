#include "Renderer.h"
#include "SceneReader.h"
#include "Timer.h"

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
	SceneReader sceneReader("../scene/13.scene");
	Renderer renderer{ sceneReader.getViewer(), sceneReader.getScene() };

	Timer timer;
	timer.begin();
	printf("Rendering . . .\n");
	Mat img = renderer.render(true);
	printf("Rendering duration: %.4lfs\n", timer.getDuration());

	imshow("Ray Tracing", img);
	imwrite("../img/test.png", img);
	waitKey(0);

	return 0;
}