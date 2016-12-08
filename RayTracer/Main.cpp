#include "DataGenerator.h"
#include "MatReader.h"
#include "NeuralNetwork.h"
#include "Renderer.h"
#include "SceneReader.h"
#include "Timer.h"
#include "TravelManager.h"

#include <opencv2/opencv.hpp>

#include <array>
#include <fstream>

using namespace cv;
using namespace std;

enum STATUS { TRAVEL, RENDER, GENERATE };

int main(int argc, char *argv[]) {
	string sceneName;
	string saveName;
	STATUS status;

	if (argc == 3 || argc == 4) {
		sceneName = argv[1];

		if (!strcmp(argv[2], "-travel")) status = TRAVEL;
		else if (!strcmp(argv[2], "-render")) status = RENDER;
		else if (!strcmp(argv[2], "-generate")) status = GENERATE;
		else error_exit("Error at argument 2!\n");

		if (argc == 4) saveName = argv[3];
		else saveName = "../img/test";
	} else error_exit("Input arguments are wrong!\n");

	SceneReader sceneReader(sceneName);
	if (status == TRAVEL) {
		Renderer renderer{ sceneReader };
		TravelManager manager;
		manager.travel(renderer);
	} else if (status == RENDER) {
		Renderer renderer{ sceneReader };
		Timer timer;
		array<Mat, 3> img = renderer.render(true);
		printf("Render duration: %.4lfs\n", timer.getDuration());

		ostringstream oss;
		oss << saveName << ".png";
		imwrite(oss.str(), img[0]);

		cout << "Image is saved at " << saveName << endl;
	} else if (status == GENERATE) {
		Timer timer;
		DataGenerator dataGenerator{ sceneReader, 15, 1200, 10000 };
		dataGenerator.generateTrainData();
		cout << "Duration: " << timer.getDuration() << "s" << endl;
	}

	return 0;
}