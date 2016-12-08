#pragma once

#include "Scene.h"
#include "SceneReader.h"
#include "TraceBase.h"
#include "Viewer.h"

#include <opencv2/opencv.hpp>

#include <array>

class Renderer {
private:
	Viewer viewer;
	const Scene &scene;
	std::shared_ptr<TraceBase> tracer;
	TraceType traceType;

public:
	explicit Renderer(const SceneReader &reader) :
		viewer(reader.viewer), scene(reader.scene), tracer(reader.tracer), traceType(reader.traceType) {}
	~Renderer() {}

	std::array<cv::Mat, 3> render(bool showBar = false) const;
	Viewer &getViewer() { return viewer; }

private:
	std::array<cv::Mat, 3> rawRender(bool showBar) const;
	cv::Mat double2uchar(const cv::Mat &img) const;
};