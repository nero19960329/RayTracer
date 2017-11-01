#pragma once

#include <glm.hpp>

#include <opencv2/opencv.hpp>

#include <memory>

#include "material.h"
#include "utils.h"

class Texture {
protected:
	std::shared_ptr<Material> material;

public:
	explicit Texture(const Material &material_) { material = std::make_shared<Material>(material_); }
	virtual ~Texture() {}

	virtual std::shared_ptr<Material> getProp() const { return nullptr; }
	virtual std::shared_ptr<Material> getProp(double x, double y) const = 0;
};

class PureTexture : public Texture {
public:
	PureTexture(const Material & material_, glm::dvec3 color = zero_vec3);

	std::shared_ptr<Material> getProp() const override { return material; }
	std::shared_ptr<Material> getProp(double x, double y) const override { return nullptr; }
};

class GridTexture : public Texture {
private:
	std::shared_ptr<Material> white, black;
	int dim;

public:
	GridTexture(const Material & material_, int dim_ = 8);

	std::shared_ptr<Material> getProp(double x, double y) const override {
		int tmp = (int)(std::floor(x * dim) + std::floor(y * dim));
		if (tmp % 2)
			return white;
		else
			return black;
	}
};

class ImageTexture : public Texture {
private:
	cv::Mat rgbMat;
	int dim;

public:
	ImageTexture(const Material &material_, const std::string &_inputFileName, int _dim = 4) :
		Texture(material_), dim(_dim) {
		cv::Mat image = cv::imread(_inputFileName);
		if (!image.rows || image.rows != image.cols || image.type() != CV_8UC3) return;

		rgbMat = cv::Mat{ image.rows, image.cols, CV_64FC3 };
		image.convertTo(rgbMat, CV_64FC3, 1.0 / 255, 0.0);
	}

	std::shared_ptr<Material> getProp(double x, double y) const override;
};