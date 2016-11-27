#pragma once

#include "Scene.h"
#include "SceneReader.h"
#include "Viewer.h"

#include <opencv2/opencv.hpp>

class Renderer {
private:
	Viewer viewer;
	const Scene &scene;
	TraceType traceType;
	BRDFType brdfType;

public:
	explicit Renderer(const SceneReader &reader) :
		viewer(reader.viewer), scene(reader.scene), traceType(reader.traceType), brdfType(reader.brdfType) {}

	Renderer(const Viewer &_viewer, const Scene &_scene, TraceType _traceType, BRDFType _brdfType) : 
		viewer(_viewer), scene(_scene), traceType(_traceType), brdfType(_brdfType) {}
	~Renderer() {}

	cv::Mat render(bool showBar = false) const;
	Viewer &getViewer() { return viewer; }

private:
	cv::Mat rawRender(bool showBar) const;
	void normalize(cv::Mat &img) const;
	cv::Mat double2uchar(const cv::Mat &img) const;
};