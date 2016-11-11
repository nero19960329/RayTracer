#include "KDTree.h"

#include <functional>

using namespace std;

real_t KDNode::SAH(SplitPlane &plane, const AABB &aabb, array<int, 3> cnts) {
	AABB leftBox, rightBox;
	aabb.splitBox(plane, leftBox, rightBox);
	real_t SA = aabb.halfSA(), invSA = 1.0 / SA;
	real_t leftRatio = leftBox.halfSA() * invSA;
	real_t rightRatio = rightBox.halfSA() * invSA;
	real_t costLeft = C(leftRatio, rightRatio, cnts[0] + cnts[2], cnts[1]);
	real_t costRight = C(leftRatio, rightRatio, cnts[0], cnts[1] + cnts[2]);
	if (costLeft < costRight) {
		plane.side = LEFT_SIDE;
		return costLeft;
	} else {
		plane.side = RIGHT_SIDE;
		return costRight;
	}
}

vector<KDNode::ObjSide> KDNode::ClassifyLeftRightBoth(size_t objLen, const vector<Event> &events, const SplitPlane &plane) {
	vector<ObjSide> res{ objLen, BOTH };
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

vector<KDNode::Event> KDNode::GetEvents(const vector<shared_ptr<Object>> &objs, const AABB &aabb) {
	vector<Event> res;
	rep(k, 3) {
		rep(i, objs.size()) {
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

SplitPlane KDNode::FindPlane(int objLen, const AABB &aabb, const vector<Event> &events, real_t &minCost) {
	SplitPlane bestPlane{ -1, 0.0, NONE_SIDE };

	array<array<int, 3>, 3> cnts;
	rep(k, 3) {
		cnts[k][0] = 0;				// left
		cnts[k][1] = objLen;		// right
		cnts[k][2] = 0;				// lying
	}

	auto eventLen = events.size();
	for (auto i = 0; i < eventLen;) {
		SplitPlane plane{ events[i].axis, events[i].split, NONE_SIDE };
		array<int, 3> pCnts = { 0, 0, 0 };	// ending, lying, starting
		rep(j, 3) {
			while (i < eventLen && events[i].axis == plane.axis && events[i].split == plane.value && events[i].type == j) {
				++pCnts[j];
				++i;
			}
		}

		array<int, 3> &nowCnts = cnts[plane.axis];
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

shared_ptr<KDNode> KDNode::build(const vector<shared_ptr<Object>> &_objs, const AABB &aabb, const vector<Event> &events, const SplitPlane &lastPlane, int depth) {
	shared_ptr<KDNode> node = make_shared<KDNode>();
	node->left = nullptr;
	node->right = nullptr;
	node->aabb = aabb;

	if (_objs.size() < MIN_KDNODE_OBJS) {
		for (const auto &obj : _objs) node->objs.push_back(obj);
		return node;
	}

	real_t minCost = numeric_limits<real_t>::max();
	SplitPlane bestPlane = FindPlane(_objs.size(), aabb, events, minCost);

	if (minCost >= KDTREE_SAH_KI * _objs.size() || bestPlane.axis == -1 || bestPlane == lastPlane) {
		for (const auto &obj : _objs) node->objs.push_back(obj);
		return node;
	}

	vector<ObjSide> objSides = ClassifyLeftRightBoth(_objs.size(), events, bestPlane);
	vector<shared_ptr<Object>> leftObjs, rightObjs;
	vector<size_t> leftMap(_objs.size(), numeric_limits<size_t>::max());
	vector<size_t> rightMap(_objs.size(), numeric_limits<size_t>::max());
	rep(i, _objs.size()) {
		if (objSides[i] == LEFT_ONLY) {
			leftMap[i] = leftObjs.size();
			leftObjs.push_back(_objs[i]);
		} else if (objSides[i] == RIGHT_ONLY) {
			rightMap[i] = rightObjs.size();
			rightObjs.push_back(_objs[i]);
		} else {
			leftMap[i] = leftObjs.size();
			rightMap[i] = rightObjs.size();
			leftObjs.push_back(_objs[i]);
			rightObjs.push_back(_objs[i]);
		}
	}

	vector<Event> leftOnlyEvents, rightOnlyEvents;
	vector<Event> bothLeftEvents, bothRightEvents;
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
					assert(false);
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
	sort(bothLeftEvents.begin(), bothLeftEvents.end(), eventCmp);
	sort(bothRightEvents.begin(), bothRightEvents.end(), eventCmp);

	vector<Event> leftEvents, rightEvents;
	merge(leftOnlyEvents.begin(), leftOnlyEvents.end(),
		bothLeftEvents.begin(), bothLeftEvents.end(),
		back_inserter(leftEvents), eventCmp);
	merge(rightOnlyEvents.begin(), rightOnlyEvents.end(),
		bothRightEvents.begin(), bothRightEvents.end(),
		back_inserter(rightEvents), eventCmp);

	AABB leftBox, rightBox;
	aabb.splitBox(bestPlane, leftBox, rightBox);
	node->left = build(leftObjs, leftBox, leftEvents, bestPlane, depth + 1);
	node->right = build(rightObjs, rightBox, rightEvents, bestPlane, depth + 1);

	return node;
}

KDTree::KDTree(const vector<shared_ptr<Object>> &_objs) :
	Object(nullptr) {
	Timer timer;
	timer.begin();
	AABB aabb;
	for (const auto &obj : _objs) aabb.expand(obj->getAABB());
	vector<KDNode::Event> eventVec = root->GetEvents(_objs, aabb);
	auto eventCmp = [](const KDNode::Event &a, const KDNode::Event &b) {
		return a.split < b.split || (a.split == b.split && a.type < b.type);
	};
	sort(eventVec.begin(), eventVec.end(), eventCmp);
	root = KDNode::build(_objs, aabb, eventVec);
	printf("Building duration: %.4lfs\n", timer.getDuration());
}

shared_ptr<Intersect> KDTree::getTrace(const Ray &ray, real_t dist) const {
	auto res = make_shared<KDTreeIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

bool KDTreeIntersect::isIntersect(const shared_ptr<KDNode> &node, const Ray &ray) const {
	if (node->left && node->right) {
		real_t t[2];
		bool leftHit = node->left->aabb.intersect(ray, t[0]);
		bool rightHit = node->right->aabb.intersect(ray, t[1]);

		if (!leftHit && !rightHit) return false;
		else if (leftHit && !rightHit) return isIntersect(node->left, ray);
		else if (!leftHit && rightHit) return isIntersect(node->right, ray);
		else {
			if (t[0] < t[1]) return isIntersect(node->left, ray) || isIntersect(node->right, ray);
			else return isIntersect(node->right, ray) || isIntersect(node->left, ray);
		}
	} else {
		bool flag = false;
		for (const auto &obj : node->objs) {
			auto intersect = obj->getTrace(ray);
			if (intersect) {
				if (updateMin(distToInter, intersect->getDistToInter())) {
					normal = intersect->getNormal();
					surface = intersect->getSurface();
				}
				flag = true;
			}
		}
		return flag;
	}
}

bool KDTreeIntersect::isIntersect() const {
	if (tree.root->aabb.intersect(ray)) return isIntersect(tree.root, ray);
	else return false;
}

real_t KDTreeIntersect::getDistToInter() const {
	return distToInter;
}

Vec3 KDTreeIntersect::getNormal() const {
	return normal;
}

shared_ptr<Surface> KDTreeIntersect::getInterPointSurfaceProperty() const {
	return nullptr;
}