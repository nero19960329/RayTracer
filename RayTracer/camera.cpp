#include "camera.h"
#include "constants.h"

Camera::Camera(int width_, int height_, glm::dvec3 center_, glm::dvec3 target, glm::dvec3 up_, double fovy_) :
	width(width_), height(height_), center(center_), dir(target - center_), up(up_), fovy(fovy_) {
	dir = glm::normalize(dir);
	up = glm::normalize(up);
	up = glm::normalize(glm::cross(glm::cross(dir, up), dir));

	double w2, h2;
	h2 = std::tan(fovy * PI / 360.0);
	w2 = h2 * width * 1.0 / height;

	x = glm::cross(dir, up);
	y = up;

	glm::dvec3 halfW, halfH;
	halfW = w2 * x;
	halfH = h2 * y;
	LT = center + dir - halfW + halfH;
	deltaW = (halfW + halfW) / (width * 1.0);
	deltaH = (halfH + halfH) / (height * 1.0);
}

RaySampler::RaySampler(const Camera & camera_) :
	cam(camera_), rtMode(false), mcptMode(false), jitterMode(false), jitterIdx(0),
	dopMode(false), apertureSize(5e-3), focusOffset(1.0) {}

Ray RaySampler::ray(int i, int j) const {
	glm::dvec3 projectCenter = cam.LT - cam.deltaH * (j * 1.0) + cam.deltaW * (i * 1.0);
	return Ray(cam.center, glm::normalize(projectCenter - cam.center));
}

Ray RaySampler::jitterRay(int i, int j) const {
	int sample = jitterSampleNum;

	int q = jitterIdx % sample, p = jitterIdx / sample;

	glm::dvec3 tmpVec = cam.LT - cam.deltaH * (j * 1.0) + cam.deltaW * (i * 1.0) - 
		cam.deltaH * (q * 1.0 / sample) + cam.deltaW * (p * 1.0 / sample);
	glm::dvec3 rayCenter, rayDir;

	tmpVec -= rng.randomDouble() * cam.deltaH / (sample * 1.0);
	tmpVec += rng.randomDouble() * cam.deltaW / (sample * 1.0);

	rayCenter = cam.center;
	rayDir = tmpVec - rayCenter;

	jitterIdx = (jitterIdx + 1) % (sample * sample);

	return Ray(rayCenter, glm::normalize(rayDir));
}

Ray RaySampler::dopRay(int i, int j) const {
	glm::dvec3 tmpVec = cam.LT - cam.deltaH * (j * 1.0) + cam.deltaW * (i * 1.0);
	glm::dvec3 rayCenter, rayDir;

	tmpVec -= rng.randomDouble() * cam.deltaH;
	tmpVec += rng.randomDouble() * cam.deltaW;
	
	glm::dvec3 focusPoint = cam.center + glm::normalize(tmpVec - cam.center) * focusOffset;
	double tmpRadius = rng.randomDouble() * apertureSize;
	double tmpTheta = rng.randomDouble() * 360.0;
	rayCenter = cam.center + tmpRadius * (std::cos(tmpTheta) * cam.x + std::sin(tmpTheta) * cam.y);
	rayDir = focusPoint - rayCenter;

	return Ray(rayCenter, glm::normalize(rayDir));
}

Ray RaySampler::randomRay(int i, int j) const {
	glm::dvec3 tmpVec = cam.LT - cam.deltaH * (j * 1.0) + cam.deltaW * (i * 1.0);
	tmpVec -= rng.randomDouble() * cam.deltaH;
	tmpVec += rng.randomDouble() * cam.deltaW;
	return Ray(cam.center, glm::normalize(tmpVec - cam.center));
}
