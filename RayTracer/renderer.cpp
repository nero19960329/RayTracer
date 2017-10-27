#include "renderer.h"

#include <functional>
#include <omp.h>
#include <random>

Renderer::Renderer(const Camera & camera_, TraceBase * tracer_, const RaySampler & sampler_) :
	camera(camera_), tracer(tracer_), sampler(sampler_) {}

cv::Mat Renderer::render() const {
	std::function<glm::dvec3(const Renderer &, int, int)> kernel;
	if (sampler.rtMode == true) {
		if (sampler.dopMode == true) kernel = &Renderer::rayTracingDopKernel;
		else if (sampler.jitterMode == true) kernel = &Renderer::rayTracingJitterKernel;
		else kernel = &Renderer::rayTracingTrivialKernel;
	} else if (sampler.mcptMode == true) {
		kernel = &Renderer::mcptTrivialKernel;
	}

	std::vector<int> widthVec, heightVec;
	for (int i = 0; i < camera.width; ++i) widthVec.push_back(i);
	for (int j = 0; j < camera.height; ++j) heightVec.push_back(j);

	std::random_device rng;
	std::mt19937 urng(rng());
	std::shuffle(widthVec.begin(), widthVec.end(), urng);
	std::shuffle(heightVec.begin(), heightVec.end(), urng);

	cv::Mat res = cv::Mat::zeros(camera.height, camera.width, CV_64FC3);
	#pragma omp parallel for
	for (int i = 0; i < camera.width; ++i) {
		for (int j = 0; j < camera.height; ++j) {
			int i_ = widthVec[i], j_ = heightVec[j];
			glm::dvec3 color = kernel(*this, i_, j_);
			res.at<cv::Vec3d>(j_, i_)[0] += color[2];
			res.at<cv::Vec3d>(j_, i_)[1] += color[1];
			res.at<cv::Vec3d>(j_, i_)[2] += color[0];
		}
	}

	cv::Mat finalRes{ camera.height, camera.width, CV_8UC3 };
	res.convertTo(finalRes, CV_8UC3, 255, 0.5);
	return finalRes;
}

glm::dvec3 Renderer::rayTracingTrivialKernel(int i, int j) const {
	Ray ray = sampler.ray(i, j);
	return tracer->color(ray);
}

glm::dvec3 Renderer::rayTracingJitterKernel(int i, int j) const {
	int sample2 = sampler.jitterSampleNum * sampler.jitterSampleNum;
	glm::dvec3 color = zero_vec3;
	for (int k = 0; k < sample2; ++k) {
		Ray ray = sampler.jitterRay(i, j);
		color += tracer->color(ray) / (sample2 * 1.0);
	}
	return color;
}

glm::dvec3 Renderer::rayTracingDopKernel(int i, int j) const {
	int sample = sampler.dopSampleNum;
	glm::dvec3 color = zero_vec3;
	for (int k = 0; k < sample; ++k) {
		Ray ray = sampler.dopRay(i, j);
		color += tracer->color(ray) / (sample * 1.0);
	}
	return color;
}

glm::dvec3 Renderer::mcptTrivialKernel(int i, int j) const {
	int sample = sampler.mcptSampleNum;
	glm::dvec3 color = zero_vec3;
	for (int k = 0; k < sample; ++k) {
		Ray ray = sampler.randomRay(i, j);
		color += clamp(tracer->color(ray, &rng)) / (sample * 1.0);
	}
	return color;
}
