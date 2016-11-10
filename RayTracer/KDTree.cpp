#include "KDTree.h"

#include <functional>

using namespace std;

shared_ptr<KDNode> KDNode::build(vector<shared_ptr<Object>> &_objs, const AABB &aabb, int depth) {
	shared_ptr<KDNode> node = make_shared<KDNode>();
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