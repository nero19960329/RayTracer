#pragma once

#include "camera.h"
#include "scene.h"
#include "trace_base.h"

#include <opencv2/opencv.hpp>

class Renderer {
private:
	const Camera & camera;
	TraceBase * tracer;
	const RaySampler & sampler;
	mutable RNG rng;

public:
	Renderer(const Camera & camera_, TraceBase * tracer_, const RaySampler & sampler_);

	cv::Mat render() const;

private:
	glm::dvec3 rayTracingTrivialKernel(int i, int j) const;
	glm::dvec3 rayTracingJitterKernel(int i, int j) const;
	glm::dvec3 rayTracingDopKernel(int i, int j) const;
	glm::dvec3 mcptTrivialKernel(int i, int j) const;
};