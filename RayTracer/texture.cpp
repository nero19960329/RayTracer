#include "bsdf.h"
#include "texture.h"

PureTexture::PureTexture(const Material & material_, glm::dvec3 color /* = zero_vec3 */) :
	Texture(material_) { material->bsdf->setColor(color); }

GridTexture::GridTexture(const Material & material_, int dim_ /* = 8 */) :
	Texture(material_), dim(dim_) {
	white = Material::clone(material);
	white->bsdf->setColor(glm::dvec3(1.0, 1.0, 1.0));
	black = Material::clone(material);
	black->bsdf->setColor(glm::dvec3(0.0, 0.0, 0.0));
}

std::shared_ptr<Material> ImageTexture::getProp(double x, double y) const {
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