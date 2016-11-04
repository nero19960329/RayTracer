#pragma once

#include "Surface.h"

#include <memory>

class Texture {
public:
	Texture() {}
	virtual ~Texture() {}

	virtual std::shared_ptr<Surface> getSurfaceProperty() const = 0;
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

	std::shared_ptr<Surface> getSurfaceProperty() const {
		return surface;
	}

	std::shared_ptr<Surface> getSurfaceProperty(real_t x, real_t y) const {
		assert(false);
		return nullptr;
	}
};