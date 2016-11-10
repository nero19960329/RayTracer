#include "KDTree.h"

#include <functional>

using namespace std;

real_t KDNode::SAH(const SplitPlane &plane, const AABB &aabb, array<int, 3> cnts, bool &side) {
	AABB leftBox, rightBox;
	aabb.splitBox(plane, leftBox, rightBox);
	real_t SA = aabb.halfSA(), invSA = 1.0 / SA;
	real_t leftRatio = leftBox.halfSA() * invSA;
	real_t rightRatio = rightBox.halfSA() * invSA;
	real_t costLeft = C(leftRatio, rightRatio, cnts[0] + cnts[2], cnts[1]);
	real_t costRight = C(leftRatio, rightRatio, cnts[0], cnts[1] + cnts[2]);
	if (costLeft < costRight) {
		side = LEFT_SIDE;
		return costLeft;
	} else {
		side = RIGHT_SIDE;
		return costRight;
	}
}

array<SplitPlane, 6> KDNode::PerfectSplits(const shared_ptr<Object> &obj, const AABB &aabb) {
	AABB B = obj->getAABB();
	rep(i, 3) {
		updateMax(B.bounds[0][i], aabb.bounds[0][i]);
		updateMin(B.bounds[1][i], aabb.bounds[1][i]);
	}
	array<SplitPlane, 6> res;
	rep(i, 3) {
		res[i + i] = SplitPlane{ i, B.bounds[0][i] };
		res[i + i + 1] = SplitPlane{ i, B.bounds[1][i] };
	}
	return res;
}

array<vector<shared_ptr<Object>>, 3> KDNode::Classify(const vector<shared_ptr<Object>> &objs, const AABB &left, const AABB &right, const SplitPlane &plane) {
	array<vector<shared_ptr<Object>>, 3> res;
	for (const auto &obj : objs) {
		if (obj->liesInPlane(plane)) res[2].push_back(obj);
		else {
			AABB objAABB = obj->getAABB();
			if (objAABB.bounds[0][plane.axis] <= plane.value) res[0].push_back(obj);
			if (objAABB.bounds[1][plane.axis] >= plane.value) res[1].push_back(obj);
		}
	}
	return res;
}

shared_ptr<KDNode> KDNode::build(vector<shared_ptr<Object>> &_objs, const AABB &aabb, int depth) {
	/*shared_ptr<KDNode> node = make_shared<KDNode>();
	node->left = nullptr;
	node->right = nullptr;
	node->aabb = aabb;

	if (_objs.size() < MIN_KDNODE_OBJS) {
		for (const auto &obj : _objs) node->objs.push_back(obj);
		return node;
	}

	int axis = node->aabb.getLongestAxis();
	function<bool(const shared_ptr<Object> &, const shared_ptr<Object> &)> cmp = 
		[=](const shared_ptr<Object> &elem, const shared_ptr<Object> &tmp) { return elem->getAABB().getMid()[axis] < tmp->getAABB().getMid()[axis]; };
	auto median = findKthElement(_objs, _objs.size() / 2, cmp);

	vector<shared_ptr<Object>> leftObjs, rightObjs;
	real_t split = (*median)->getAABB().getMid()[axis];

	repa(it, _objs.begin(), median) {
		leftObjs.push_back(*it);
		if ((*it)->getAABB().bounds[1][axis] > split) rightObjs.push_back(*it);
	}
	repa(it, median, _objs.end()) {
		rightObjs.push_back(*it);
		if ((*it)->getAABB().bounds[0][axis] < split) leftObjs.push_back(*it);
	}

	auto dupCnt = leftObjs.size() + rightObjs.size() - _objs.size();
	real_t dupScale = dupCnt * 1.0 / _objs.size();
	if (dupScale < DUPLICATE_RATIO && leftObjs.size() != _objs.size() && rightObjs.size() != _objs.size()) {
		AABB leftAABB = aabb, rightAABB = aabb;
		leftAABB.bounds[1][axis] = split;
		rightAABB.bounds[0][axis] = split;
		node->left = build(leftObjs, leftAABB, depth + 1);
		node->right = build(rightObjs, rightAABB, depth + 1);
	} else {
		for (const auto &obj : _objs) node->objs.push_back(obj);
	}

	return node;*/

	shared_ptr<KDNode> node = make_shared<KDNode>();
	node->left = nullptr;
	node->right = nullptr;
	node->aabb = aabb;

	if (_objs.size() < MIN_KDNODE_OBJS) {
		for (const auto &obj : _objs) node->objs.push_back(obj);
		return node;
	}

	real_t minCost = numeric_limits<real_t>::max();
	bool sideFlag = NONE_SIDE;
	SplitPlane plane;
	AABB leftBox, rightBox;

	for (const auto &obj : _objs) {
		array<SplitPlane, 6> splitPlanes = PerfectSplits(obj, aabb);
		for (const auto &splitPlane : splitPlanes) {
			AABB tmpLeftBox, tmpRightBox;
			aabb.splitBox(splitPlane, tmpLeftBox, tmpRightBox);
			array<vector<shared_ptr<Object>>, 3> splitObjs = Classify(_objs, tmpLeftBox, tmpRightBox, splitPlane);
			bool tmpSideFlag = NONE_SIDE;
			array<int, 3> splitCnts = { splitObjs[0].size(), splitObjs[1].size(), splitObjs[2].size() };
			real_t cost = SAH(splitPlane, aabb, splitCnts, tmpSideFlag);
			if (cost < minCost) {
				minCost = cost;
				sideFlag = tmpSideFlag;
				plane = splitPlane;
				leftBox = tmpLeftBox;
				rightBox = tmpRightBox;
			}
		}
	}

	array<vector<shared_ptr<Object>>, 3> splitObjs = Classify(_objs, leftBox, rightBox, plane);

	if (sideFlag == LEFT_SIDE) splitObjs[0].insert(splitObjs[0].end(), splitObjs[2].begin(), splitObjs[2].end());
	else splitObjs[1].insert(splitObjs[1].end(), splitObjs[2].begin(), splitObjs[2].end());

	//auto dupCnt = splitObjs[0].size() + splitObjs[1].size() - _objs.size();
	//real_t dupScale = dupCnt * 1.0 / _objs.size();
	//if (dupScale < DUPLICATE_RATIO && splitObjs[0].size() != _objs.size() && splitObjs[1].size() != _objs.size()) {
	if (minCost < KDTREE_SAH_KI * _objs.size()) {
		/*printf("left: %d, right: %d, all: %d", splitObjs[0].size(), splitObjs[1].size(), _objs.size());
		getchar();*/
		node->left = build(splitObjs[0], leftBox, depth + 1);
		node->right = build(splitObjs[1], rightBox, depth + 1);
	} else {
		for (const auto &obj : _objs) node->objs.push_back(obj);
	}

	return node;
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