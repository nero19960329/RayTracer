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
	PureTexture(const Material & material_, glm::dvec3 color = zero_vec3) :
		Texture(material_) { material->bsdf->setColor(color); }

	std::shared_ptr<Material> getProp() const override { return material; }
	std::shared_ptr<Material> getProp(double x, double y) const override { return nullptr; }
};

class GridTexture : public Texture {
private:
	std::shared_ptr<Material> white, black;
	int dim;

public:
	GridTexture(const Material & material_, int dim_ = 8) :
		Texture(material_), dim(dim_) {
		white = Material::clone(material);
		white->bsdf->setColor(glm::dvec3(1.0, 1.0, 1.0));
		black = Material::clone(material);
		black->bsdf->setColor(glm::dvec3(0.0, 0.0, 0.0));
	}

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

	std::shared_ptr<Material> getProp(double x, double y) const override {
		x /= dim;
		y /= dim;
		double tmpX = x - floor(x), tmpY = y - floor(y);
		int row = (int)(tmpX * rgbMat.rows);
		int col = (int)(tmpY * rgbMat.cols);
		cv::Vec3d rgb = rgbMat.at<cv::Vec3d>(row, col);

		auto pixelMaterial = Material::clone(material);
		pixelMaterial->bsdf->setColor(glm::dvec3(rgb[2], rgb[1], rgb[0]));
		return pixelMaterial;
	}
};

/*
class ImageTexture : public Texture {
private:
	cv::Mat rgbMat;
	int dim;

public:
	ImageTexture(const Material &_material, const std::string &_inputFileName, int _dim = 4) :
		Texture(_material), dim(_dim) {
		cv::Mat image = cv::imread(_inputFileName);

		if (!image.rows || image.rows != image.cols || image.type() != CV_8UC3)
			return;

		rgbMat = cv::Mat{ image.rows, image.cols, CV_64FC3 };
		image.convertTo(rgbMat, CV_64FC3, 1.0 / 255, 0.0);

		double maxDiffuse = 0.0;
		for (int i = 0; i < rgbMat.rows; ++i) {
			for (int j = 0; j < rgbMat.cols; ++j) {
				cv::Vec3d rgb = rgbMat.at<cv::Vec3d>(i, j);
				updateMax(maxDiffuse, rgb[0]);
				updateMax(maxDiffuse, rgb[1]);
				updateMax(maxDiffuse, rgb[2]);
			}
		}
		if (maxDiffuse + material.specular > 1.0 - ABSORB_COEFFICIENT) {
			rgbMat *= (1.0 - ABSORB_COEFFICIENT) * maxDiffuse / (maxDiffuse + material.specular);
			material.specular *= (1.0 - ABSORB_COEFFICIENT) / (maxDiffuse + material.specular);
		}
	}
	~ImageTexture() {}

	std::shared_ptr<Material> getProp(double x, double y) const override {
		x /= dim;
		y /= dim;
		double tmpX = x - floor(x), tmpY = y - floor(y);
		int row = (int)(tmpX * rgbMat.rows);
		int col = (int)(tmpY * rgbMat.cols);
		cv::Vec3d rgb = rgbMat.at<cv::Vec3d>(row, col);
		return std::make_shared<Material>(material, glm::dvec3(rgb[2], rgb[1], rgb[0]));
	}
};*/