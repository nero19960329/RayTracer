#include "Renderer.h"
#include "SceneReader.h"
#include "Timer.h"
#include "TravelManager.h"

#include <opencv2/opencv.hpp>
#include <fstream>

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {
	srand((unsigned) time(NULL));

	string sceneName;
	string saveName;
	bool travelFlag = false;

	if (argc == 2) {
		sceneName = argv[1];
	} else if (argc == 3 || argc == 4) {
		sceneName = argv[1];
		if (!strcmp(argv[2], "-travel")) travelFlag = true;
		else if (!strcmp(argv[2], "-render")) travelFlag = false;
		else error_exit("Error at argument 2!\n");

		if (argc == 4) saveName = argv[3];
		else saveName = "../img/test.png";
	} else {
		error_exit("Input arguments are wrong!\n");
	}

	SceneReader sceneReader(sceneName);
	Renderer renderer{ sceneReader.getViewer(), sceneReader.getScene(), sceneReader.getTraceType() };

	if (travelFlag) {
		TravelManager manager;
		manager.travel(renderer);
	} else {
		Timer timer;
		Mat img = renderer.render(true);
		printf("Render duration: %.4lfs\n", timer.getDuration());

		//imshow("Ray Tracing", img);
		imwrite(saveName, img);
		cout << "Image is saved at " << saveName << endl;
		//waitKey(0);
	}

	return 0;
}