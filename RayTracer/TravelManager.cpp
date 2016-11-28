#include "TravelManager.h"

#include <opencv2/opencv.hpp>

using namespace cv;

const int M_KEY_LEFT = 2424832;
const int M_KEY_RIGHT = 2555904;
const int M_KEY_UP = 2490368;
const int M_KEY_DOWN = 2621440;

void TravelManager::travel(Renderer &renderer) const {
	bool exitFlag = false, renderFlag = false;
	Mat img = renderer.render()[0];
	while (!exitFlag) {
		if (renderFlag) img = renderer.render()[0];
		imshow("Ray Tracing", img);
		int key = waitKey(100);
		renderFlag = true;
		switch (key) {
		case 'w':
			renderer.getViewer().moveForward(step); break;
		case 's':
			renderer.getViewer().moveForward(-step); break;
		case 'a':
			renderer.getViewer().moveSide(-step); break;
		case 'd':
			renderer.getViewer().moveSide(step); break;
		case M_KEY_LEFT:		// LEFT
			renderer.getViewer().rotateSide(-thetaStep); break;
		case M_KEY_RIGHT:		// RIGHT
			renderer.getViewer().rotateSide(thetaStep); break;
		case M_KEY_UP:			// UP
			renderer.getViewer().rotateUp(thetaStep); break;
		case M_KEY_DOWN:		// DOWN
			renderer.getViewer().rotateUp(-thetaStep); break;
		case 't':
			renderer.getViewer().changeProjectionType(); break;
		case 'q':
			exitFlag = true; break;
		default:
			renderFlag = false; break;
		}
	}
}