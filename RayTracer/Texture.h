#pragma once

#include "Constants.h"
#include "Surface.h"

#include <memory>

#include <opencv2/opencv.hpp>

class Texture {
protected:
	Material material;

public:
	Texture(const Material &_material) : material(_material) {}
	virtual ~Texture() {}

	virtual std::shared_ptr<Surface> getSurfaceProperty() const {
		return nullptr;
	}

	virtual std::shared_ptr<Surface> getSurfaceProperty(real_t x, real_t y) const = 0;
};

class PureTexture : public Texture {
private:
	Vec3 color;
	std::shared_ptr<Surface> surface;

public:
	explicit PureTexture(const Material &_material, const Vec3 &_color) :
		Texture(_material) {
		surface = std::make_shared<Surface>(_material, _color);
	}
	~PureTexture() {}

	std::shared_ptr<Surface> getSurfaceProperty() const override {
		return surface;
	}

	std::shared_ptr<Surface> getSurfaceProperty(real_t x, real_t y) const override {
		assert(false);
		return nullptr;
	}
};

class GridTexture : public Texture {
private:
	std::shared_ptr<Surface> white, black;
	int dim;

public:
	explicit GridTexture(const Material &_material, int _dim = 8) :
		Texture(_material), dim(_dim) {}
	~GridTexture() {}

	std::shared_ptr<Surface> getSurfaceProperty(real_t x, real_t y) const override {
		int tmp = (int) (floor(x * dim) + floor(y * dim));
		if (tmp % 2) {
			return std::make_shared<Surface>(material, Vec3::WHITE);
		} else {
			return std::make_shared<Surface>(material, Vec3::BLACK);
		}
	}
};

class ImageTexture : public Texture {
private:
	cv::Mat rgbMat;

public:
	explicit ImageTexture(const Material &_material, const std::string &_inputFileName) :
		Texture(_material) {
		cv::Mat image = cv::imread(_inputFileName);

		if (!image.rows) {
			error_exit("Texture image not found!\n");
		} else if (image.rows != image.cols) {
			error_exit("Texture image is not square!\n");
		} else if (image.type() != CV_8UC3) {
			error_exit("Should input a 8UC3 image!\n");
		}

		rgbMat = cv::Mat{ image.rows, image.cols, CV_64FC3 };
		image.convertTo(rgbMat, CV_64FC3, 1.0 / 255, 0.0);
	}
	~ImageTexture() {}

	std::shared_ptr<Surface> getSurfaceProperty(real_t x, real_t y) const override {
		real_t tmpX = x - floor(x), tmpY = y - floor(y);
		int row = (int) (tmpX * rgbMat.rows);
		int col = (int) (tmpY * rgbMat.cols);
		cv::Vec3d rgb = rgbMat.at<cv::Vec3d>(row, col);
		return std::make_shared<Surface>(material, Vec3{ rgb[2], rgb[1], rgb[0] });
	}
};