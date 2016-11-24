#include "Renderer.h"
#include "SceneReader.h"
#include "Timer.h"
#include "TravelManager.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#include <opencv2/opencv.hpp>
#include <fstream>

using namespace cv;
using namespace std;
using namespace rapidxml;

int main(int argc, char *argv[]) {
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

	file<> fdoc(sceneName.c_str());
	xml_document<> doc;
	doc.parse<0>(fdoc.data());

	xml_node<> *scene = doc.first_node("scene");
	cout << scene->name() << endl;
	xml_node<> *viewer = scene->first_node("viewer");
	cout << viewer->name() << endl;
	xml_node<> *test = viewer->first_node();
	cout << test->name() << endl;
	cout << test->first_attribute("type")->value() << endl;
	cout << test->first_attribute("value")->value() << endl;

	/*SceneReader sceneReader(sceneName);
	Renderer renderer{ sceneReader.getViewer(), sceneReader.getScene() };

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
	}*/

	return 0;
}