#include "DataGenerator.h"
#include "Renderer.h"
#include "SceneReader.h"
#include "Timer.h"
#include "TravelManager.h"

#include <opencv2/opencv.hpp>

#include <array>
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
		else saveName = "../img/test";
	} else {
		error_exit("Input arguments are wrong!\n");
	}

	SceneReader sceneReader(sceneName);

	/*DataGenerator dataGenerator{ sceneReader.getScene(), 10, 6000, 40, 7, { -0.5, 0.0, -0.5 }, { 0.5, 1.0, 0.5 } };
	dataGenerator.generateTrainData();*/

	Renderer renderer{ sceneReader };

	if (travelFlag) {
		TravelManager manager;
		manager.travel(renderer);
	} else {
		Timer timer;
		array<Mat, 3> img = renderer.render(true);
		printf("Render duration: %.4lfs\n", timer.getDuration());

		//imshow("Ray Tracing", img);
		ostringstream oss;
		oss << saveName << "_direct.png";
		imwrite(oss.str(), img[0]);
		oss.str("");
		oss << saveName << "_indirect.png";
		imwrite(oss.str(), img[1]);
		oss.str("");
		oss << saveName << "_all.png";
		imwrite(oss.str(), img[2]);

		cout << "Image is saved at " << saveName << endl;
		//waitKey(0);
	}

	return 0;
}