#include "ProgressPrinter.h"
#include "Renderer.h"
#include "TraceBase.h"

#include <omp.h>

using namespace cv;
using namespace std;

Mat Renderer::render( bool showBar) const {
	Mat img = rawRender(showBar);
	normalize(img);
	return double2uchar(img);
}

Mat Renderer::rawRender(bool showBar) const {
	auto shader = scene.getTracingType(type);
	Geometry screen = viewer.getScreen();

	Mat res = Mat::zeros(screen.height, screen.width, CV_64FC3);
	int antiSample2 = sqr(viewer.antiSample);
	int sample = viewer.dopSample * antiSample2;
	long long allRays = (long long) screen.width * (long long) screen.height * (long long) sample;

	vector<int> widthVec, heightVec;
	rep(i, screen.width) widthVec.emplace_back(i);
	rep(j, screen.height) heightVec.emplace_back(j);
	random_shuffle(widthVec.begin(), widthVec.end());
	random_shuffle(heightVec.begin(), heightVec.end());

	if (showBar) {
		omp_lock_t lock;
		omp_init_lock(&lock);

		ProgressPrinter printer{ "Rendering => ", allRays };
		printer.start();
		#pragma omp parallel for
		for (long long k = 0; k < allRays; ++k) {
			int i = widthVec[(k / sample) / screen.height], j = heightVec[(k / sample) % screen.height];
			int p = ((k / viewer.dopSample) % antiSample2) / viewer.antiSample, q = ((k / viewer.dopSample) % antiSample2) % viewer.antiSample;
			Vec3 color = shader->color(viewer.getRay(i, j, p, q));
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
		for (long long k = 0; k < allRays; ++k) {
			int i = widthVec[(k / sample) / screen.height], j = heightVec[(k / sample) % screen.height];
			int p = ((k / viewer.dopSample) % antiSample2) / viewer.antiSample, q = ((k / viewer.dopSample) % antiSample2) % viewer.antiSample;
			Vec3 color = shader->color(viewer.getRay(i, j, p, q));
			res.at<Vec3d>(j, i)[0] += color[2];
			res.at<Vec3d>(j, i)[1] += color[1];
			res.at<Vec3d>(j, i)[2] += color[0];
		}
	}

	rep(i, res.rows) rep(j, res.cols) res.at<Vec3d>(i, j) /= sample;

	return res;
}

void Renderer::normalize(Mat &img) const {
	//double maxValue;

	rep(i, img.rows) rep(j, img.cols) {
		Vec3d &rgb = img.at<Vec3d>(i, j);
		rgb = Vec3d{ cut(rgb[0]), cut(rgb[1]), cut(rgb[2]) };
	}

	/*if (maxValue > 1.0f) {
		rep(i, img.rows) rep(j, img.cols) {
			img.at<Vec3d>(i, j) /= maxValue;
		}
	}*/
}

Mat Renderer::double2uchar(const Mat &img) const {
	Mat res{ img.rows, img.cols, CV_8UC3 };
	img.convertTo(res, CV_8UC3, 255, 0.5);
	return res;
}