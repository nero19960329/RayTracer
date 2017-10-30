#pragma once

#include "object.h"

#include <vector>

class KDTree;
class KDTreeIntersect;

class KDNode {
	friend class KDTree;
	friend class KDTreeIntersect;

protected:
	AABB aabb;
	KDNode *left, *right;
	std::vector<Object *> objs;

	enum EventType { ENDING = 0, LYING, STARTING };
	enum ObjSide { BOTH, LEFT_ONLY, RIGHT_ONLY };
	struct Event {
		double split;
		EventType type;
		int axis;
		size_t objID;

		Event(double split_, EventType type_, int axis_, size_t objID_) :
			split(split_), type(type_), axis(axis_), objID(objID_) {}
	};

public:
	KDNode() {}

	static KDNode * build(const std::vector<Object *> &objs, const AABB &aabb, const std::vector<Event> &events, int threads = 1, const SplitPlane &lastPlane = {}, int depth = 0);

private:
	static std::vector<ObjSide> classifyObjSide(size_t objSize, const std::vector<Event> &events, const SplitPlane &plane);

	static inline double C(double leftRatio, double rightRatio, int leftCnt, int rightCnt);
	static double SAH(SplitPlane &plane, const AABB &aabb, int cnts[3]);
	static SplitPlane findPlane(int objSize, const AABB &aabb, const std::vector<Event> &events, double &minCost);
	static std::vector<Event> getEvents(const std::vector<Object *> &objs, const AABB &aabb);
};

class KDTree : public Object {
	friend class KDTreeIntersect;

public:
	KDNode *root;

public:
	explicit KDTree(const std::vector<Object *> &objs);

	std::shared_ptr<Intersect> getTrace(const Ray &ray, double dist = std::numeric_limits<double>::max()) const override;
	AABB getAABB() const override { return root->aabb; }
};

class KDTreeIntersect : public Intersect {
private:
	const KDTree &tree;

	mutable glm::dvec3 normal;
	mutable double refrIdx;
	mutable std::shared_ptr<Material> material;
	mutable bool insideFlag;

	virtual std::shared_ptr<Material> getInterPointMaterialProp() const override;

public:
	KDTreeIntersect(const KDTree &_tree, const Ray &_ray) :
		Intersect(_ray), tree(_tree) { distToInter = std::numeric_limits<double>::max(); }
	~KDTreeIntersect() {}

	const Object *getObj() const override { return &tree; }
	double getDistToInter() const override;
	bool isIntersect() const override;
	glm::dvec3 getNormal() const override;
	IntersectInfo getIntersectInfo() override { return IntersectInfo{ getIntersection(), getNormal(), getMaterial(), getNextRefrIdx() }; }

private:
	std::shared_ptr<Material> getMaterial() const override { return material; }
	bool isIntersect(KDNode *node, const Ray &ray) const;
	bool leafIntersect(const std::vector<Object *> &objs) const;
	double getNextRefrIdx() const override { return refrIdx; }
};