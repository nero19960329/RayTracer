#include <algorithm>
#include <omp.h>

#include "kdtree.h"

KDNode * KDNode::build(const std::vector<Object *> &objs, const AABB &aabb, const std::vector<Event> &events, int threads, const SplitPlane &lastPlane, int depth) {
	KDNode *node = new KDNode;
	node->left = nullptr;
	node->right = nullptr;
	node->aabb = aabb;

	if (objs.size() < MIN_KDNODE_OBJS || depth >= MAX_KDTREE_DEPTH) {
		for (const auto &obj : objs) node->objs.push_back(obj);
		return node;
	}

	double minCost = std::numeric_limits<double>::max();
	SplitPlane bestPlane = findPlane(int(objs.size()), aabb, events, minCost);

	if (minCost >= KDTREE_SAH_KI * objs.size() || bestPlane.axis == -1 || bestPlane == lastPlane) {
		for (const auto &obj : objs) node->objs.push_back(obj);
		return node;
	}

	std::vector<ObjSide> objSides = classifyObjSide(objs.size(), events, bestPlane);
	std::vector<Object *> leftObjs, rightObjs;
	std::vector<size_t> leftMap(objs.size(), std::numeric_limits<size_t>::max());
	std::vector<size_t> rightMap(objs.size(), std::numeric_limits<size_t>::max());
	for (int i = 0; i < objs.size(); ++i) {
		if (objSides[i] == LEFT_ONLY) {
			leftMap[i] = leftObjs.size();
			leftObjs.push_back(objs[i]);
		} else if (objSides[i] == RIGHT_ONLY) {
			rightMap[i] = rightObjs.size();
			rightObjs.push_back(objs[i]);
		} else {
			leftMap[i] = leftObjs.size();
			rightMap[i] = rightObjs.size();
			leftObjs.push_back(objs[i]);
			rightObjs.push_back(objs[i]);
		}
	}

	std::vector<Event> leftOnlyEvents, rightOnlyEvents;
	std::vector<Event> bothLeftEvents, bothRightEvents;
	for (const auto &e : events) {
		if (objSides[e.objID] == LEFT_ONLY) leftOnlyEvents.emplace_back(e.split, e.type, e.axis, leftMap[e.objID]);
		else if (objSides[e.objID] == RIGHT_ONLY) rightOnlyEvents.emplace_back(e.split, e.type, e.axis, rightMap[e.objID]);
		else {
			if (e.axis == bestPlane.axis) {
				if (e.type == STARTING) {
					bothLeftEvents.emplace_back(e.split, e.type, e.axis, leftMap[e.objID]);
					bothRightEvents.emplace_back(bestPlane.value, e.type, e.axis, rightMap[e.objID]);
				} else if (e.type == ENDING) {
					bothLeftEvents.emplace_back(bestPlane.value, e.type, e.axis, leftMap[e.objID]);
					bothRightEvents.emplace_back(e.split, e.type, e.axis, rightMap[e.objID]);
				} else {
					// assert(false);
				}
			} else {
				// can modify!!!!
				bothLeftEvents.emplace_back(e.split, e.type, e.axis, leftMap[e.objID]);
				bothRightEvents.emplace_back(e.split, e.type, e.axis, rightMap[e.objID]);
			}
		}
	}

	auto eventCmp = [](const Event &a, const Event &b) {
		return a.split < b.split || (a.split == b.split && a.type < b.type);
	};
	std::sort(bothLeftEvents.begin(), bothLeftEvents.end(), eventCmp);
	std::sort(bothRightEvents.begin(), bothRightEvents.end(), eventCmp);

	std::vector<Event> leftEvents, rightEvents;
	std::merge(leftOnlyEvents.begin(), leftOnlyEvents.end(),
		bothLeftEvents.begin(), bothLeftEvents.end(),
		back_inserter(leftEvents), eventCmp);
	std::merge(rightOnlyEvents.begin(), rightOnlyEvents.end(),
		bothRightEvents.begin(), bothRightEvents.end(),
		back_inserter(rightEvents), eventCmp);

	AABB leftBox, rightBox;
	aabb.splitBox(bestPlane, leftBox, rightBox);

	if (threads > 1) {
		#pragma omp parallel sections
		{
			#pragma omp section
			{
				node->left = build(leftObjs, leftBox, leftEvents, threads / 2, bestPlane, depth + 1);
			}
			#pragma omp section
			{
				node->right = build(rightObjs, rightBox, rightEvents, threads - threads / 2, bestPlane, depth + 1);
			}
		}
	} else {
		node->left = build(leftObjs, leftBox, leftEvents, 1, bestPlane, depth + 1);
		node->right = build(rightObjs, rightBox, rightEvents, 1, bestPlane, depth + 1);
	}

	return node;
}

std::vector<KDNode::ObjSide> KDNode::classifyObjSide(size_t objSize, const std::vector<Event> &events, const SplitPlane &plane) {
	std::vector<ObjSide> res{ objSize, BOTH };
	for (const auto &e : events) {
		if (e.type == ENDING && e.axis == plane.axis && e.split <= plane.value) res[e.objID] = LEFT_ONLY;
		else if (e.type == STARTING && e.axis == plane.axis && e.split >= plane.value) res[e.objID] = RIGHT_ONLY;
		else if (e.type == LYING && e.axis == plane.axis) {
			if (e.split < plane.value || (e.split == plane.value && plane.side == LEFT_SIDE)) res[e.objID] = LEFT_ONLY;
			if (e.split > plane.value || (e.split == plane.value && plane.side == RIGHT_SIDE)) res[e.objID] = RIGHT_ONLY;
		}
	}
	return res;
}

inline double KDNode::C(double leftRatio, double rightRatio, int leftCnt, int rightCnt) {
	double res = KDTREE_SAH_KT + KDTREE_SAH_KI * (leftRatio * leftCnt + rightRatio * rightCnt);
	if (!leftCnt || !rightCnt) return 0.8 * res;
	else return res;
}

double KDNode::SAH(SplitPlane &plane, const AABB &aabb, int cnts[3]) {
	AABB leftBox, rightBox;
	aabb.splitBox(plane, leftBox, rightBox);
	double SA = aabb.halfSA(), invSA = 1.0 / SA;
	double leftRatio = leftBox.halfSA() * invSA;
	double rightRatio = rightBox.halfSA() * invSA;
	double costLeft = C(leftRatio, rightRatio, cnts[0] + cnts[2], cnts[1]);
	double costRight = C(leftRatio, rightRatio, cnts[0], cnts[1] + cnts[2]);
	if (costLeft < costRight) {
		plane.side = LEFT_SIDE;
		return costLeft;
	} else {
		plane.side = RIGHT_SIDE;
		return costRight;
	}
}

SplitPlane KDNode::findPlane(int objSize, const AABB &aabb, const std::vector<Event> &events, double &minCost) {
	SplitPlane bestPlane{ -1, 0.0, NONE_SIDE };

	int cnts[3][3];
	for (int k = 0; k < 3; ++k) {
		cnts[k][0] = 0;				// left
		cnts[k][1] = objSize;		// right
		cnts[k][2] = 0;				// lying
	}

	auto eventLen = events.size();
	for (auto i = 0; i < eventLen; ) {
		SplitPlane plane{ events[i].axis, events[i].split, NONE_SIDE };
		int pCnts[3] = { 0, 0, 0 };	// ending, lying, starting
		for (int j = 0; j < 3; ++j) {
			while (i < eventLen && events[i].axis == plane.axis && events[i].split == plane.value && events[i].type == j) {
				++pCnts[j];
				++i;
			}
		}

		int *nowCnts = cnts[plane.axis];
		nowCnts[2] = pCnts[LYING];
		nowCnts[1] -= pCnts[LYING];
		nowCnts[1] -= pCnts[ENDING];
		if (updateMin(minCost, SAH(plane, aabb, nowCnts))) bestPlane = plane;
		nowCnts[0] += pCnts[STARTING];
		nowCnts[0] += pCnts[LYING];
		nowCnts[2] = 0;
	}

	return bestPlane;
}

std::vector<KDNode::Event> KDNode::getEvents(const std::vector<Object*> &objs, const AABB &aabb) {
	std::vector<Event> res;
	for (int k = 0; k < 3; ++k) {
		for (int i = 0; i < objs.size(); ++i) {
			AABB box = objs[i]->clipToBox(aabb);
			if (box.isPlanar(k)) res.emplace_back(box.bounds[0][k], LYING, k, i);
			else {
				res.emplace_back(box.bounds[0][k], STARTING, k, i);
				res.emplace_back(box.bounds[1][k], ENDING, k, i);
			}
		}
	}
	return res;
}

KDTree::KDTree(const std::vector<Object*> &objs) : Object(nullptr) {
	AABB aabb;
	for (const auto &obj : objs) {
		aabb.expand(obj->getAABB());
	}
	std::vector<KDNode::Event> eventVec = root->getEvents(objs, aabb);
	auto eventCmp = [](const KDNode::Event &a, const KDNode::Event &b) {
		return a.split < b.split || (a.split == b.split && a.type < b.type);
	};
	sort(eventVec.begin(), eventVec.end(), eventCmp);
	root = KDNode::build(objs, aabb, eventVec, omp_get_num_procs());
}

std::shared_ptr<Intersect> KDTree::getTrace(const Ray &ray, double dist) const {
	auto res = std::make_shared<KDTreeIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

std::shared_ptr<Surface> KDTreeIntersect::getInterPointSurfaceProp() const {
	return nullptr;
}

double KDTreeIntersect::getDistToInter() const {
	return distToInter;
}

bool KDTreeIntersect::isIntersect() const {
	if (tree.root->aabb.intersect(ray)) return isIntersect(tree.root, ray);
	else return false;
}

glm::dvec3 KDTreeIntersect::getNormal() const {
	return normal;
}

bool KDTreeIntersect::isIntersect(KDNode *node, const Ray &ray) const {
	if (node->left && node->right) {
		double t[2];
		bool leftHit = node->left->aabb.intersect(ray, t[0]);
		bool rightHit = node->right->aabb.intersect(ray, t[1]);

		if (leftHit && !rightHit) return isIntersect(node->left, ray);
		else if (!leftHit && rightHit) return isIntersect(node->right, ray);
		else if (leftHit && rightHit) {
			if (t[0] < t[1]) return isIntersect(node->left, ray) || isIntersect(node->right, ray);
			else return isIntersect(node->right, ray) || isIntersect(node->left, ray);
		} else return false;
	} else return leafIntersect(node->objs);
}

bool KDTreeIntersect::leafIntersect(const std::vector<Object*> &objs) const {
	bool flag = false;
	for (const auto &obj : objs) {
		auto intersect = obj->getTrace(ray);
		if (intersect) {
			if (updateMin(distToInter, intersect->getDistToInter())) {
				normal = intersect->getNormal();
				surface = intersect->getSurface();
				refrIdx = intersect->getNextRefrIdx();
				insideFlag = intersect->getObj()->hasInside();
			}
			flag = true;
		}
	}
	return flag;
}
