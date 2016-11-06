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

	virtual Vec3 color(const Ray &ray) const = 0;
};

class Phong : public Shader {
private:
	int maxDepth;

public:
	explicit Phong(const Scene &_scene, int _maxDepth = MAX_TRACING_DEPTH) : Shader(_scene), maxDepth(_maxDepth) {}
	~Phong() {}

	Vec3 color(const Ray &ray) const override;

private:
	Vec3 getColor(DistRay &ray, int depth = 0) const;
	Vec3 getPhongLocal(const IntersectInfo &info, const DistRay &ray, int depth = 0) const;
	Vec3 getReflection(const IntersectInfo &info, const DistRay &ray, int depth = 0) const;

	bool isShadow(const std::shared_ptr<Light> &light, const IntersectInfo &info) const;
};