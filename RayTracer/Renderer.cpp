#include "Renderer.h"

#include "ProgressPrinter.h"

#include <omp.h>

using namespace cv;
using namespace std;

Mat Renderer::render(int sample, bool showBar) const {
	Mat img = rawRender(sample, showBar);
	normalize(img);
	return float2uchar(img);
}

Mat Renderer::rawRender(int sample, bool showBar) const {
	auto shader = scene.getShader(type);
	auto rays = viewer.getRayVector(sample);
	Geometry screen = viewer.getScreen();

	Mat res = Mat::zeros(screen.height, screen.width, CV_64FC3);
	int allRays = screen.width * screen.height * sample;

	vector<int> widthVec, heightVec;
	rep(i, screen.width) widthVec.emplace_back(i);
	rep(j, screen.height) heightVec.emplace_back(j);
	random_shuffle(widthVec.begin(), widthVec.end());
	random_shuffle(heightVec.begin(), heightVec.end());

	if (showBar) {
		omp_lock_t lock;
		omp_init_lock(&lock);

		ProgressPrinter printer{ "Rendering => ", allRays };
		printer.display(0);
		#pragma omp parallel for
		for (int k = 0; k < allRays; ++k) {
			int i = widthVec[(k / sample) / screen.height], j = heightVec[(k / sample) % screen.height];
			Vec3 color = shader->color(rays[i][j][k % sample]);
			res.at<Vec3d>(j, i)[0] += color[2];
			res.at<Vec3d>(j, i)[1] += color[1];
			res.at<Vec3d>(j, i)[2] += color[0];

			omp_set_lock(&lock);
			printer.display(1);
			omp_unset_lock(&lock);
		}
		printer.finish();
		omp_destroy_lock(&lock);
	} else {
		#pragma omp parallel for
		for (int k = 0; k < allRays; ++k) {
			int i = widthVec[(k / sample) / screen.height], j = heightVec[(k / sample) % screen.height];
			Vec3 color = shader->color(rays[i][j][k % sample]);
			res.at<Vec3d>(j, i)[0] += color[2];
			res.at<Vec3d>(j, i)[1] += color[1];
			res.at<Vec3d>(j, i)[2] += color[0];
		}
	}

	rep(i, res.rows) rep(j, res.cols) res.at<Vec3d>(i, j) /= sample;

	return res;
}

Mat Renderer::renderWithBar(int allRays, const vector<int> &widthVec, const vector<int> &heightVec) const {

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