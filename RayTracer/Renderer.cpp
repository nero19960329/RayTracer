#include "Renderer.h"

using namespace cv;
using namespace std;

Mat Renderer::render() const {
	Mat img = rawRender();
	normalize(img);
	return float2uchar(img);
}

Mat Renderer::rawRender() const {
	auto shader = scene.getShader(type);
	auto rays = viewer.getRayVector();
	Geometry screen = viewer.getScreen();

	Mat res{ screen.height, screen.width, CV_64FC3 };
	int i = 0, j, allPixels = screen.width * screen.height;
	for (const auto &rayVec : rays) {
		printProgress(i * screen.height * 1.0 / allPixels);
		j = 0;
		for (const auto &ray : rayVec) {
			Vec3 color = shader->color(ray);
			res.at<Vec3d>(j, i)[0] = color[2];
			res.at<Vec3d>(j, i)[1] = color[1];
			res.at<Vec3d>(j, i)[2] = color[0];
			++j;
		}
		++i;
	}
	printProgress(1);
	printf("\n");

	return res;
}

void Renderer::normalize(Mat &img) const {
	double maxValue;
	rep(i, img.rows) rep(j, img.cols) {
		Vec3d rgb = img.at<Vec3d>(i, j);
		rep(k, 3) updateMax(maxValue, rgb[k]);
	}

	if (maxValue > 1.0f) {
		rep(i, img.rows) rep(j, img.cols) {
			img.at<Vec3d>(i, j) /= maxValue;
		}
	}
}

Mat Renderer::float2uchar(const Mat &img) const {
	Mat res{ img.rows, img.cols, CV_8UC3 };
	img.convertTo(res, CV_8UC3, 255, 0.5);
	return res;
}