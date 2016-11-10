#pragma once

#include "AABB.h"
#include "Constants.h"
#include "Object.h"
#include "Timer.h"

#undef max

#include <memory>
#include <vector>

class KDTree;
class KDTreeIntersect;

class KDNode {
	friend class KDTree;
	friend class KDTreeIntersect;

protected:
	AABB aabb;
	std::shared_ptr<KDNode> left, right;
	std::vector<std::shared_ptr<Object>> objs;

public:
	KDNode() {}
	~KDNode() {}

	static std::shared_ptr<KDNode> build(std::vector<std::shared_ptr<Object>> &_objs, const AABB &aabb, int depth = 0);
};

class KDTree : public Object {
	friend class KDTreeIntersect;

public:
	std::shared_ptr<KDNode> root;

public:
	explicit KDTree(std::vector<std::shared_ptr<Object>> &_objs) : Object(nullptr) {
		AABB aabb;
		for (const auto &obj : _objs) aabb.expand(obj->getAABB());
		Timer timer;
		timer.begin();
		root = KDNode::build(_objs, aabb);
		printf("Building duration: %.4lfs\n", timer.getDuration());
	}
	~KDTree() {}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;

	AABB getAABB() const override {
		return root->aabb;
	}
};

class KDTreeIntersect : public Intersect {
private:
	const KDTree &tree;

	mutable Vec3 normal;
	mutable std::shared_ptr<Surface> surface;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const override;

	const Object *getObj() const override { return &tree; }

public:
	KDTreeIntersect(const KDTree &_tree, const Ray &_ray) : Intersect(_ray), tree(_tree) {
		distToInter = std::numeric_limits<real_t>::max();
	}
	~KDTreeIntersect() {}

	real_t getDistToInter() const override;
	bool isIntersect() const override;
	Vec3 getNormal() const override;

private:

	std::shared_ptr<Surface> getSurface() const override {
		return surface;
	}

	bool isIntersect(const std::shared_ptr<KDNode> &node, const Ray &ray) const;
};