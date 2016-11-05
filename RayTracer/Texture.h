#pragma once

#include "Surface.h"

#include <memory>

#include <opencv2/opencv.hpp>

class Texture {
public:
	Texture() {}
	virtual ~Texture() {}

	virtual std::shared_ptr<Surface> getSurfaceProperty() const {
		return nullptr;
	}

	virtual std::shared_ptr<Surface> getSurfaceProperty(real_t x, real_t y) const = 0;
};

class PureTexture : public Texture {
private:
	std::shared_ptr<Surface> surface;

public:
	explicit PureTexture(const Surface &_surface) {
		surface = std::make_shared<Surface>(_surface);
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
	explicit GridTexture(int _dim = 8) :
		dim(_dim), white(std::make_shared<Surface>(Surface::WHITE)), black(std::make_shared<Surface>(Surface::BLACK)) {}
	~GridTexture() {}

	std::shared_ptr<Surface> getSurfaceProperty(real_t x, real_t y) const override {
		int tmp = (int) (floor(x * dim) + floor(y * dim));
		if (tmp % 2) {
			return white;
		} else {
			return black;
		}
	}
};

class ImageTexture : public Texture {
private:
	cv::Mat image;

public:
	explicit ImageTexture(std::string inputFileName) {
		image = cv::imread(inputFileName);
		assert(image.rows);
		assert(image.rows == image.cols);
		assert(image.type() == CV_8UC3);
	}
	~ImageTexture() {}

	std::shared_ptr<Surface> getSurfaceProperty(real_t x, real_t y) const override {
		real_t tmpX = x - floor(x), tmpY = y - floor(y);
		cv::Vec3b rgb = image.at<cv::Vec3b>((int) (tmpX * image.rows), (int) (tmpY * image.cols));
		return std::make_shared<Surface>(0.0, Vec3{ rgb[2] * 1.0 / 255, rgb[1] * 1.0 / 255, rgb[0] * 1.0 / 255 }, 0.2, 5, Vec3::BLACK);
	}
};