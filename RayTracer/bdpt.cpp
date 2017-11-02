#include "bdpt.h"

#include <gtx/norm.hpp>

#include <iostream>

glm::dvec3 BidirectionalPathTracing::color(const Ray & ray, RNG * rng) const {
	Ray startRay(ray);
	return getColor(startRay, rng);
}

glm::dvec3 BidirectionalPathTracing::getColor(Ray & ray, RNG * rng) const {
	auto eyeNodes = getEyePathNodes(ray, rng);
	auto lightNodes = getLightPathNodes(rng);

	if (eyeNodes.size() <= 1) return zero_vec3;

	glm::dvec3 color = zero_vec3;

	int s = eyeNodes.size();
	int t = lightNodes.size();
	int K = s + t - 1;
	//bool tmp = false;
	//if (t >= 2 && eyeNodes[1].info.interPoint.y != 1.585) tmp = true;
	for (int k = 1; k < K; ++k) {
		int start = std::max(1, k - t + 1);
		int end = std::min(s, k + 1);

		glm::dvec3 c = zero_vec3;
		double probSum = 0.0;
		for (int i = start; i < end; ++i) {
			auto &[f, p] = evalPath(rng, eyeNodes, i, lightNodes, k - i);
			probSum += p;
			c += f;
			/*if (tmp) {
				std::cout << "(" << i << ", " << (k - i) << ") -> " << std::endl;
				std::cout << "E" << i << " : " << eyeNodes[i].info.interPoint.x << ", " << eyeNodes[i].info.interPoint.y << ", " << eyeNodes[i].info.interPoint.z << std::endl;
				std::cout << "L" << (k - i) << " : " << lightNodes[k - i].info.interPoint.x << ", " << lightNodes[k - i].info.interPoint.y << ", " << lightNodes[k - i].info.interPoint.z << std::endl;
				std::cout << f.x << ", " << f.y << ", " << f.z << " / " << p << " => " << (f / p).x << ", " << (f / p).y << ", " << (f / p).z << std::endl;
			}*/
		}

		/*if (k + 1 < s && eyeNodes[k + 1].info.material->emission != zero_vec3) {
			auto &[f, p] = evalPath(rng, eyeNodes, k + 1, lightNodes, -1);
			probSum += p;
			c += f;
		}*/

		/*if (tmp) {
			std::cout << (c / probSum).x << ", " << (c / probSum).y << ", " << (c / probSum).z << std::endl;
			std::cout << "-----------------" << std::endl;
		}*/

		if (probSum > 0.0)
			color += c / probSum;
	}
	/*if (tmp) {
		std::cout << "color = " << color.x << ", " << color.y << ", " << color.z << std::endl;

		std::cout << "=======================" << std::endl;
		getchar();
	}*/
	/*color = zero_vec3;
	for (int i = 1; i < s; ++i) {
		auto &[f, p] = evalPath(rng, eyeNodes, i, lightNodes, 0);
		color += f / p;
	}
	std::cout << "color = " << color.x << ", " << color.y << ", " << color.z << std::endl;
	getchar();
	std::cout << "=======================" << std::endl;*/

	return color;
}

std::vector<PathNode> BidirectionalPathTracing::getEyePathNodes(Ray & startRay, RNG * rng) const {
	std::vector<PathNode> res;

	IntersectInfo info;
	info.interPoint = startRay.ori;
	PathNode node(nullptr, info, one_vec3, 1.0);
	res.push_back(node);

	auto intersect = scene.getIntersect(startRay);
	if (!intersect) return res;
	info = intersect->getIntersectInfo();
	node = PathNode(intersect->getObj(), info, one_vec3, 1.0);
	res.push_back(node);

	return getPathNodes(res, startRay, rng);
}

std::vector<PathNode> BidirectionalPathTracing::getLightPathNodes(RNG * rng) const {
	std::vector<PathNode> res;

	glm::dvec3 outDir;
	auto &[lightIdx, ratio] = scene.lightIdxSample(*rng);
	auto sampleLight = scene.lights[lightIdx];
	glm::dvec3 samplePoint = sampleLight->luminaireSample(*rng);
	double p = ratio * sampleLight->pdf(*rng);
	IntersectInfo info;
	info.interPoint = samplePoint;
	info.material = sampleLight->getTexture()->getProp();
	info.nextRefrIdx = VACUUM_REFRACTION_INDEX;
	info.normal = sampleLight->getNormal(samplePoint);
	glm::dvec3 f = info.material->emission;
	PathNode lastNode(sampleLight, info, f, p);
	res.push_back(lastNode);

	glm::dvec3 inDir = zero_vec3;
	auto sampler = info.material->bsdf->getSampler(Ray(), info);
	Ray ray = sampler->sample(rng);
	auto newIntersect = scene.getIntersect(ray);
	if (!newIntersect) return res;
	auto newInfo = newIntersect->getIntersectInfo();
	double G = glm::dot(info.normal, ray.dir) * glm::dot(newInfo.normal, -ray.dir) / glm::length2(info.interPoint - newInfo.interPoint);
	f *= G;
	p *= sampler->pdf_ortho() * G;
	lastNode = PathNode(newIntersect->getObj(), newInfo, f, p);
	res.push_back(lastNode);

	return getPathNodes(res, ray, rng);
}

std::vector<PathNode> BidirectionalPathTracing::getPathNodes(const std::vector<PathNode> & nodes, Ray & startRay, RNG * rng) const {
	std::vector<PathNode> res(nodes);

	Ray ray(startRay), newRay(startRay);
	double q;
	for (int idx = 2; ; ++idx) {
		q = 1.0;
		if (idx >= minDepth) {
			q = RUSSIAN_ROULETTE_POSSIBILITY;
			if (rng->randomDouble() >= q) return res;
		}

		auto lastNode = res.back();
		auto sampler = lastNode.info.material->bsdf->getSampler(ray, lastNode.info);
		newRay = sampler->sample(rng);
		auto newIntersect = scene.getIntersect(newRay);
		if (!newIntersect) return res;
		auto newInfo = newIntersect->getIntersectInfo();

		double cos_theta1 = glm::dot(lastNode.info.normal, newRay.dir);
		double cos_theta2 = glm::dot(newInfo.normal, -newRay.dir);
		if (cos_theta1 < 0.0 && cos_theta2 > 0.0) cos_theta1 = -cos_theta1;	// transmission
		else if (cos_theta1 > 0.0 && cos_theta2 < 0.0) return res;			// wrong condition

		double G = cos_theta1 * cos_theta2 / glm::length2(lastNode.info.interPoint - newInfo.interPoint);
		glm::dvec3 f = lastNode.f * sampler->eval() * G;
		double p = lastNode.p * q * sampler->pdf_ortho() * G;
		PathNode node(newIntersect->getObj(), newInfo, f, p);
		res.push_back(node);

		ray = newRay;
	}
}

std::tuple<glm::dvec3, double> BidirectionalPathTracing::evalPath(RNG * rng, const std::vector<PathNode>& eyeNodes, int eyeIdx, const std::vector<PathNode>& lightNodes, int lightIdx) const {
	int i = eyeIdx, j = lightIdx;

	glm::dvec3 f = zero_vec3;
	double p = 0.0;
	if (j == -1) {
		glm::dvec3 Ei_1Ei = glm::normalize(eyeNodes[i].info.interPoint - eyeNodes[i - 1].info.interPoint);
		if (glm::dot(Ei_1Ei, eyeNodes[i].info.normal) < 0.0) {
			f = eyeNodes[i].info.material->emission * eyeNodes[i].f;
			p = eyeNodes[i].p;
		}
	} else if (j == 0) {
		glm::dvec3 Ei_1Ei = glm::normalize(eyeNodes[i].info.interPoint - eyeNodes[i - 1].info.interPoint);
		glm::dvec3 EiLj = glm::normalize(lightNodes[0].info.interPoint - eyeNodes[i].info.interPoint);

		Ray shadowRay(eyeNodes[i].info.interPoint + EiLj * eps, EiLj);
		auto intersect = scene.getIntersect(shadowRay);
		if (intersect && glm::length2(intersect->getIntersectInfo().interPoint - lightNodes[0].info.interPoint) >= eps)
			return std::make_tuple(zero_vec3, 0.0);

		auto sampler = eyeNodes[i].info.material->bsdf->getSampler(-Ei_1Ei, EiLj, eyeNodes[i].info.normal, eyeNodes[i - 1].info.nextRefrIdx, VACUUM_REFRACTION_INDEX);
		double cos_theta1 = glm::dot(eyeNodes[i].info.normal, EiLj);
		double cos_theta2 = glm::dot(lightNodes[0].info.normal, -EiLj);
		if (cos_theta1 < 0.0 && cos_theta2 > 0.0) cos_theta1 = -cos_theta1;	// transmission
		else if (cos_theta1 > 0.0 && cos_theta2 < 0.0) return std::tuple<glm::dvec3, double>(f, p);	// wrong condition

		double G = cos_theta1 * cos_theta2 / glm::length2(lightNodes[0].info.interPoint - eyeNodes[i].info.interPoint);
		f = eyeNodes[i].f * G * sampler->eval() * lightNodes[0].f;
		p = eyeNodes[i].p * lightNodes[0].p;
		
	} else {
		glm::dvec3 Ei_1Ei = glm::normalize(eyeNodes[i].info.interPoint - eyeNodes[i - 1].info.interPoint);
		glm::dvec3 EiLj = glm::normalize(lightNodes[j].info.interPoint - eyeNodes[i].info.interPoint);
		glm::dvec3 LjLj_1 = glm::normalize(lightNodes[j - 1].info.interPoint - lightNodes[j].info.interPoint);

		if (glm::dot(eyeNodes[i].info.normal, EiLj) < 0.0)	// Lj and Ei may on the same object
			return std::make_tuple(zero_vec3, 0.0);

		Ray shadowRay(eyeNodes[i].info.interPoint + EiLj * eps, EiLj);
		auto intersect = scene.getIntersect(shadowRay);
		if (intersect && glm::length2(intersect->getIntersectInfo().interPoint - lightNodes[j].info.interPoint) >= eps)
			return std::make_tuple(zero_vec3, 0.0);

		double EiLjRefr = VACUUM_REFRACTION_INDEX;
		if (eyeNodes[i].obj->equals(lightNodes[j].obj) && glm::dot(EiLj, eyeNodes[i].info.normal) < 0.0 && glm::dot(-EiLj, lightNodes[j].info.normal) < 0.0)
			EiLjRefr = eyeNodes[i].obj->insideRefr();

		auto sampler1 = lightNodes[j].info.material->bsdf->getSampler(-EiLj, LjLj_1, lightNodes[j].info.normal, EiLjRefr, lightNodes[j - 1].info.nextRefrIdx);
		auto sampler2 = eyeNodes[i].info.material->bsdf->getSampler(-Ei_1Ei, EiLj, eyeNodes[i].info.normal, eyeNodes[i - 1].info.nextRefrIdx, EiLjRefr);
		double cos_theta1 = glm::dot(eyeNodes[i].info.normal, EiLj);
		double cos_theta2 = glm::dot(lightNodes[j].info.normal, -EiLj);
		if (cos_theta1 < 0.0 && cos_theta2 > 0.0) cos_theta1 = -cos_theta1;	// transmission
		else if (cos_theta1 > 0.0 && cos_theta2 < 0.0) return std::tuple<glm::dvec3, double>(f, p);	// wrong condition

		double G = cos_theta1 * cos_theta2 / glm::length2(lightNodes[j].info.interPoint - eyeNodes[i].info.interPoint);
		f = eyeNodes[i].f * sampler1->eval() * G * sampler2->eval() * lightNodes[j].f;
		p = eyeNodes[i].p * lightNodes[j].p;
	}

	return std::tuple<glm::dvec3, double>(f, p);
}
