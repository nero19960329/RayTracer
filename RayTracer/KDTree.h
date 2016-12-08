#pragma once

#include "AABB.h"
#include "Constants.h"
#include "Object.h"
#include "Timer.h"

#undef max

#include <array>
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

	enum EventType { ENDING = 0, LYING, STARTING };
	enum ObjSide { BOTH, LEFT_ONLY, RIGHT_ONLY };
	struct Event {
		real_t split;
		EventType type;
		int axis;
		size_t objID;

		Event(real_t _split, EventType _type, int _axis, size_t _objID) :
			split(_split), type(_type), axis(_axis), objID(_objID) {}
	};

public:
	KDNode() {}
	~KDNode() {}

	static std::shared_ptr<KDNode> build(const std::vector<std::shared_ptr<Object>> &_objs, const AABB &aabb, const std::vector<Event> &events, int threads = 1, const SplitPlane &lastPlane = {}, int depth = 0);

private:
	static std::vector<ObjSide> ClassifyLeftRightBoth(size_t objSize, const std::vector<Event> &events, const SplitPlane &plane);

	static inline real_t C(real_t leftRatio, real_t rightRatio, int leftCnt, int rightCnt) {
		real_t res = KDTREE_SAH_KT + KDTREE_SAH_KI * (leftRatio * leftCnt + rightRatio * rightCnt);
		if (!leftCnt || !rightCnt) return 0.8 * res;
		else return res;
	};

	static real_t SAH(SplitPlane &plane, const AABB &aabb, std::array<int, 3> cnts);
	static SplitPlane FindPlane(int objSize, const AABB &aabb, const std::vector<Event> &events, real_t &minCost);
	static std::vector<Event> GetEvents(const std::vector<std::shared_ptr<Object>> &objs, const AABB &aabb);
};

class KDTree : public Object {
	friend class KDTreeIntersect;

public:
	std::shared_ptr<KDNode> root;

public:
	explicit KDTree(const std::vector<std::shared_ptr<Object>> &_objs);
	~KDTree() {}

	std::shared_ptr<Intersect> getTrace(const Ray &ray, real_t dist = std::numeric_limits<real_t>::max()) const override;

	AABB getAABB() const override { return root->aabb; }
};

class KDTreeIntersect : public Intersect {
private:
	const KDTree &tree;

	mutable Vec3 normal;
	mutable real_t refrIdx;
	mutable int objNum;
	mutable std::shared_ptr<Surface> surface;
	mutable bool insideFlag;

	virtual std::shared_ptr<Surface> getInterPointSurfaceProperty() const override;

	const Object *getObj() const override { return &tree; }

public:
	KDTreeIntersect(const KDTree &_tree, const Ray &_ray) : 
		Intersect(_ray), tree(_tree) {
		distToInter = std::numeric_limits<real_t>::max();
	}
	~KDTreeIntersect() {}

	real_t getDistToInter() const override;
	bool isIntersect() const override;
	Vec3 getNormal() const override;

	IntersectInfo getIntersectInfo() override {
		return IntersectInfo{ getIntersection(), getNormal(), getSurface(), getNextRefractionIndex(), objNum };
	}

	bool hasInside() const override { return insideFlag; }

private:
	std::shared_ptr<Surface> getSurface() const override { return surface; }

	bool isIntersect(const std::shared_ptr<KDNode> &node, const Ray &ray) const;

	bool leafIntersect(const std::vector<std::shared_ptr<Object>> &objs) const;

	real_t getNextRefractionIndex() const override {
		return refrIdx;
	}
};