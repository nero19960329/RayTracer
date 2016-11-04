#pragma once

#include "Light.h"
#include "Object.h"

#include <list>
#include <vector>

enum ShaderType { PHONG };

class Shader;

class Scene {
public:
	std::list<std::shared_ptr<Object>> objs;
	std::vector<std::shared_ptr<Light>> lights;

public:
	Scene() {}
	virtual ~Scene() {}

	std::shared_ptr<Shader> getShader(ShaderType type) const;

	void addLight(const std::shared_ptr<Light> &light);
	void addObject(const std::shared_ptr<Object> &object);

	bool isIntersect(const Ray &ray, real_t dist) const;
	std::shared_ptr<Intersect> getIntersect(const Ray &ray) const;
};

class Shader {
protected:
	const Scene &scene;

public:
	Shader(const Scene &_scene) : scene(_scene) {}
	virtual ~Shader() {}

	virtual Vec3 getColor(const Ray &ray) const = 0;
};

class Phong : public Shader {
public:
	explicit Phong(const Scene &_scene) : Shader(_scene) {}
	~Phong() {}

	Vec3 getColor(const Ray &ray) const override;

private:
	Vec3 getPhongLocal(const IntersectInfo &info, const Ray &ray) const;
};