#include "Renderer.h"
#include "SceneReader.h"
#include "Timer.h"

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
	SceneReader sceneReader("../scene/8.scene");
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