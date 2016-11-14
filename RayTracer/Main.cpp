#include "Renderer.h"
#include "SceneReader.h"
#include "Timer.h"
#include "TravelManager.h"

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {
	string sceneName;
	bool travelFlag = false;

	if (argc == 2) {
		sceneName = argv[1];
	} else if (argc == 3) {
		sceneName = argv[1];
		if (!strcmp(argv[2], "-travel")) travelFlag = true;
		else if (!strcmp(argv[2], "-render")) travelFlag = false;
		else error_exit("Error at argument 2!\n");
	} else {
		error_exit("Input arguments are wrong!\n");
	}

	SceneReader sceneReader(sceneName);
	Renderer renderer{ sceneReader.getViewer(), sceneReader.getScene() };

	if (travelFlag) {
		TravelManager manager;
		manager.travel(renderer);
	} else {
		Timer timer;
		Mat img = renderer.render(true);
		printf("Render duration: %.4lfs\n", timer.getDuration());

		imshow("Ray Tracing", img);
		imwrite("../img/test.png", img);
		waitKey(0);
	}

	return 0;
}