#include "ProgressPrinter.h"
#include "Renderer.h"
#include "TraceBase.h"

#include <functional>
#include <omp.h>

using namespace cv;
using namespace std;

array<Mat, 3> Renderer::render(bool showBar) const {
	array<Mat, 3> imgs = rawRender(showBar);
	//normalize(img);

	array<Mat, 3> res = { double2uchar(imgs[0]), double2uchar(imgs[1]), double2uchar(imgs[2]) };
	return res;
}

array<Mat, 3> Renderer::rawRender(bool showBar) const {
	auto shader = scene.getTracer(traceType, brdfType);
	Geometry screen = viewer.getScreen();

	array<Mat, 3> results;
	int sample;
	long long allRays;

	vector<int> widthVec, heightVec;
	rep(i, screen.width) widthVec.emplace_back(i);
	rep(j, screen.height) heightVec.emplace_back(j);
	random_shuffle(widthVec.begin(), widthVec.end());
	random_shuffle(heightVec.begin(), heightVec.end());

	function<void(long long, RNGenerator *)> renderKernel;
	if (traceType == RT) {
		results[0] = Mat::zeros(screen.height, screen.width, CV_64FC3);
		Mat &res = results[0];

		int antiSample2 = sqr(viewer.antiSample);
		sample = viewer.dopSample * viewer.antiSample * viewer.antiSample;
		allRays = (long long) screen.width * (long long) screen.height * (long long) sample;

		renderKernel = [&](long long k, RNGenerator *rng) {
			int i = widthVec[(k / sample) / screen.height], j = heightVec[(k / sample) % screen.height];
			int p = ((k / viewer.dopSample) % antiSample2) / viewer.antiSample, q = ((k / viewer.dopSample) % antiSample2) % viewer.antiSample;
			pair<Vec3, Vec3> color = shader->color(viewer.getJitterSampleRay(*rng, i, j, p, q, viewer.antiSample));
			res.at<Vec3d>(j, i)[0] += color.first[2];
			res.at<Vec3d>(j, i)[1] += color.first[1];
			res.at<Vec3d>(j, i)[2] += color.first[0];
		};
	} else if (traceType == MCPT) {
		rep(k, 3) results[k] = Mat::zeros(screen.height, screen.width, CV_64FC3);

		sample = viewer.mcptSample;
		allRays = (long long) screen.width * (long long) screen.height * (long long) sample;

		renderKernel = [&](long long k, RNGenerator *rng) {
			int i = widthVec[(k / sample) / screen.height], j = heightVec[(k / sample) % screen.height];

			pair<Vec3, Vec3> radiance = shader->color(viewer.getRandomSampleRay(*rng, i, j), rng);
			Vec3 direct{ cut(radiance.first[0]), cut(radiance.first[1]), cut(radiance.first[2]) };
			Vec3 indirect{ cut(radiance.second[0]), cut(radiance.second[1]), cut(radiance.second[2]) };

			Vec3 color[3] = { direct, indirect, direct + indirect };
			rep(m, 3) {
				results[m].at<Vec3d>(j, i)[0] += color[m][2];
				results[m].at<Vec3d>(j, i)[1] += color[m][1];
				results[m].at<Vec3d>(j, i)[2] += color[m][0];
			}
		};
	}

	unsigned long long threadCnt = omp_get_max_threads();
	vector<RNGenerator *> rngs{ threadCnt };
	rep(i, threadCnt) rngs[i] = new RNGenerator(int(time(NULL)) ^ i);

	if (showBar) {
		omp_lock_t lock;
		omp_init_lock(&lock);
		ProgressPrinter printer{ "Rendering => ", allRays };
		printer.start();
		#pragma omp parallel for
		for (long long k = 0; k < allRays; ++k) {
			renderKernel(k, rngs[omp_get_thread_num()]);
			omp_set_lock(&lock);
			printer.display(1);
			omp_unset_lock(&lock);
		}
		printer.finish();
		omp_destroy_lock(&lock);
	} else {
		#pragma omp parallel for
		for (long long k = 0; k < allRays; ++k) renderKernel(k, rngs[omp_get_thread_num()]);
	}

	for (auto &res : results) {
		rep(i, res.rows) rep(j, res.cols) res.at<Vec3d>(i, j) /= sample;
	}

	return results;
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