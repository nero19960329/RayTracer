#include "bdpt.h"

#include <gtx/norm.hpp>

glm::dvec3 BidirectionalPathTracing::color(const Ray & ray, RNG * rng) const {
	DistRay distRay(ray, 0.0);
	return getColor(distRay, rng);
}

glm::dvec3 BidirectionalPathTracing::getColor(DistRay & ray, RNG * rng) const {
	auto eyeNodes = getEyePathNodes(ray, rng);
	auto lightNodes = getLightPathNodes(rng);

	if (eyeNodes.size() <= 1) return zero_vec3;

	glm::dvec3 color = zero_vec3;
	int s = eyeNodes.size();
	int t = lightNodes.size();
	int K = s + t - 1;
	for (int k = 0; k < K; ++k) {
		int start = std::max(1, k - t + 1);
		int end = std::min(s, k + 1);

		glm::dvec3 c = zero_vec3;
		double probSum = 0.0;
		for (int i = start; i < end; ++i) {
			auto &[f, p] = evalPath(rng, eyeNodes, i, lightNodes, k - i);
			probSum += p;
			c += f;
		}

		if (k + 1 < s && eyeNodes[k + 1].info.material->emission != zero_vec3) {
			auto &[f, p] = evalPath(rng, eyeNodes, k + 1, lightNodes, -1);
			probSum += p;
			c += f;
		}

		if (probSum > 0.0)
			color += c / probSum;
	}

	return color;
}

std::vector<PathNode> BidirectionalPathTracing::getEyePathNodes(DistRay & startRay, RNG * rng) const {
	std::vector<PathNode> res;

	IntersectInfo info;
	info.interPoint = startRay.ori;
	PathNode node(nullptr, info, PathNode::Eye, 0, one_vec3, 1.0);
	res.push_back(node);

	auto intersect = scene.getIntersect(startRay);
	if (!intersect) return res;
	info = intersect->getIntersectInfo();
	node = PathNode(intersect->getObj(), info, PathNode::Eye, 1, one_vec3, 1.0);
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
	PathNode lastNode(sampleLight, info, PathNode::Light, 0, f, p);
	res.push_back(lastNode);

	glm::dvec3 inDir = zero_vec3;
	BRDFSampleInfo sampleInfo(*rng, inDir, outDir, info.normal, info.material, VACUUM_REFRACTION_INDEX, VACUUM_REFRACTION_INDEX);
	brdf->brdfSample(sampleInfo);
	DistRay ray(Ray(info.interPoint + outDir * eps, outDir), sampleInfo.nextRefr);
	auto newIntersect = scene.getIntersect(ray);
	if (!newIntersect) return res;
	auto newInfo = newIntersect->getIntersectInfo();
	double G = glm::dot(info.normal, outDir) * glm::dot(newInfo.normal, -outDir) / glm::length2(info.interPoint - newInfo.interPoint);
	f *= G;
	p *= brdf->pdf(sampleInfo) * G;
	lastNode = PathNode(newIntersect->getObj(), newInfo, PathNode::Light, 1, f, p);
	res.push_back(lastNode);

	return getPathNodes(res, ray, rng);
}

std::vector<PathNode> BidirectionalPathTracing::getPathNodes(const std::vector<PathNode> & nodes, DistRay & startRay, RNG * rng) const {
	std::vector<PathNode> res(nodes);

	DistRay ray(startRay), newRay(startRay);
	double q = RUSSIAN_ROULETTE_POSSIBILITY;
	for (int idx = 2; ; ++idx) {
		q = 1.0;
		if (idx >= minDepth) {
			q = RUSSIAN_ROULETTE_POSSIBILITY;
			if (rng->randomDouble() >= q) return res;
		}

		auto lastNode = res.back();
		glm::dvec3 inDir = glm::normalize(ray.ori - lastNode.info.interPoint);
		glm::dvec3 outDir;
		BRDFSampleInfo sampleInfo(*rng, inDir, outDir, lastNode.info.normal, lastNode.info.material, ray.refrIdx, lastNode.info.nextRefrIdx);
		brdf->brdfSample(sampleInfo);
		newRay = DistRay(Ray(lastNode.info.interPoint + outDir * eps, outDir), sampleInfo.nextRefr);
		auto newIntersect = scene.getIntersect(newRay);
		if (!newIntersect) return res;
		auto newInfo = newIntersect->getIntersectInfo();

		double G = glm::dot(lastNode.info.normal, outDir) * glm::dot(newInfo.normal, -outDir) / glm::length2(lastNode.info.interPoint - newInfo.interPoint);
		if (G < 0.0) return res;
		glm::dvec3 f = lastNode.f * brdf->eval(sampleInfo) * G;
		double p = lastNode.p * q * brdf->pdf(sampleInfo) * G;
		PathNode node(newIntersect->getObj(), newInfo, PathNode::Eye, idx, f, p);
		res.push_back(node);

		lastNode = node;
		ray = newRay;
	}
}

std::tuple<glm::dvec3, double> BidirectionalPathTracing::evalPath(RNG * rng, const std::vector<PathNode>& eyeNodes, int eyeIdx, const std::vector<PathNode>& lightNodes, int lightIdx) const {
	int i = eyeIdx, j = lightIdx;

	glm::dvec3 f;
	double p;
	if (j == -1) {
		glm::dvec3 Ei_1Ei = glm::normalize(eyeNodes[i].info.interPoint - eyeNodes[i - 1].info.interPoint);
		if (glm::dot(Ei_1Ei, eyeNodes[i].info.normal) < 0.0) {
			f = eyeNodes[i].info.material->emission * eyeNodes[i].f;
			p = eyeNodes[i].p;
		} else {
			f = zero_vec3;
			p = 0.0;
		}
	} else if (j == 0) {
		glm::dvec3 Ei_1Ei = glm::normalize(eyeNodes[i].info.interPoint - eyeNodes[i - 1].info.interPoint);
		glm::dvec3 EiLj = glm::normalize(lightNodes[0].info.interPoint - eyeNodes[i].info.interPoint);

		Ray shadowRay(eyeNodes[i].info.interPoint + EiLj * eps, EiLj);
		auto intersect = scene.getIntersect(shadowRay);
		if (intersect && glm::length2(intersect->getIntersectInfo().interPoint - lightNodes[0].info.interPoint) >= eps) return std::make_tuple(zero_vec3, 1.0);

		BRDFSampleInfo sampleInfo(*rng, -Ei_1Ei, EiLj, eyeNodes[i].info.normal, eyeNodes[i].info.material, VACUUM_REFRACTION_INDEX, VACUUM_REFRACTION_INDEX);
		double G = glm::dot(eyeNodes[i].info.normal, EiLj) * glm::dot(lightNodes[0].info.normal, -EiLj) / glm::length2(lightNodes[0].info.interPoint - eyeNodes[i].info.interPoint);
		f = eyeNodes[i].f * G * brdf->eval(sampleInfo) * lightNodes[0].f;
		p = eyeNodes[i].p * lightNodes[0].p;
	} else {
		glm::dvec3 Ei_1Ei = glm::normalize(eyeNodes[i].info.interPoint - eyeNodes[i - 1].info.interPoint);
		glm::dvec3 EiLj = glm::normalize(lightNodes[j].info.interPoint - eyeNodes[i].info.interPoint);
		glm::dvec3 LjLj_1 = glm::normalize(lightNodes[j - 1].info.interPoint - lightNodes[j].info.interPoint);

		Ray shadowRay(eyeNodes[i].info.interPoint + EiLj * eps, EiLj);
		auto intersect = scene.getIntersect(shadowRay);
		if (intersect && glm::length2(intersect->getIntersectInfo().interPoint - lightNodes[j].info.interPoint) >= eps) return std::make_tuple(zero_vec3, 1.0);

		BRDFSampleInfo sampleInfo1(*rng, -EiLj, LjLj_1, lightNodes[j].info.normal, lightNodes[j].info.material, VACUUM_REFRACTION_INDEX, VACUUM_REFRACTION_INDEX);
		BRDFSampleInfo sampleInfo2(*rng, -Ei_1Ei, EiLj, eyeNodes[i].info.normal, eyeNodes[i].info.material, VACUUM_REFRACTION_INDEX, VACUUM_REFRACTION_INDEX);
		double G = glm::dot(eyeNodes[i].info.normal, EiLj) * glm::dot(lightNodes[j].info.normal, -EiLj) / glm::length2(lightNodes[j].info.interPoint - eyeNodes[i].info.interPoint);
		f = eyeNodes[i].f * brdf->eval(sampleInfo1) * G * brdf->eval(sampleInfo2) * lightNodes[j].f;
		p = eyeNodes[i].p * lightNodes[j].p;
	}

	return std::tuple<glm::dvec3, double>(f, p);
}
