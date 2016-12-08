#pragma once

#include <opencv2/opencv.hpp>

#include "DataGenerator.h"
#include "MatReader.h"
#include "Vec3.h"

class NeuralNetwork {
private:
	cv::Mat w[3], b[3];
	cv::Mat inputMin, inputMax, outputMin, outputMax;

public:
	NeuralNetwork(const MatReader &reader) {
		auto matVec = reader.readMats();
		rep(k, 3) {
			w[k] = matVec[k * 2];
			b[k] = matVec[k * 2 + 1];
		}
		inputMin = matVec[6];
		inputMax = matVec[7];
		outputMin = matVec[8];
		outputMax = matVec[9];
	}

	Vec3 predict(const DataGenerator::TrainData &inputData) const {
		cv::Mat tmpInput(15, 1, CV_64F);
		rep(k, 3) tmpInput.at<real_t>(k, 0) = inputData.hitPoint[k];
		rep(k, 3) tmpInput.at<real_t>(k + 3, 0) = inputData.viewDir[k];
		rep(k, 3) tmpInput.at<real_t>(k + 6, 0) = inputData.lightPos[k];
		rep(k, 3) tmpInput.at<real_t>(k + 9, 0) = inputData.normal[k];
		rep(k, 3) tmpInput.at<real_t>(k + 12, 0) = inputData.brdfParameter[k];

		mapminmax(tmpInput, inputMin, inputMax);

		cv::Mat input(w[0].cols, 1, CV_64F);
		int cnt = 0;
		rep(i, 15) {
			if (isfinite(tmpInput.at<real_t>(i, 0))) input.at<real_t>(cnt++, 0) = tmpInput.at<real_t>(i, 0);
		}

		cv::Mat a[2];
		a[0] = cv::Mat(b[0].rows, b[0].cols, CV_64F);
		a[0] = w[0] * input + b[0];
		std::transform(a[0].begin<real_t>(), a[0].end<real_t>(), a[0].begin<real_t>(), bind(&NeuralNetwork::tansig, this, std::placeholders::_1));
		a[1] = cv::Mat(b[1].rows, b[1].cols, CV_64F);
		a[1] = w[1] * a[0] + b[1];
		std::transform(a[1].begin<real_t>(), a[1].end<real_t>(), a[1].begin<real_t>(), bind(&NeuralNetwork::tansig, this, std::placeholders::_1));
		cv::Mat output = cv::Mat(b[2].rows, b[2].cols, CV_64F);
		output = w[2] * a[1] + b[2];

		invMapminmax(output, outputMin, outputMax);

		return{ output.at<real_t>(0, 0), output.at<real_t>(1, 0), output.at<real_t>(2, 0) };
	}

private:
	inline real_t tansig(real_t input) const { return 2.0 / (1 + exp(-2 * input)) - 1; }

	inline void mapminmax(cv::Mat &data, const cv::Mat &min, const cv::Mat &max) const {
		rep(i, data.rows) {
			if (min.at<real_t>(i, 0) == max.at<real_t>(i, 0)) data.at<real_t>(i, 0) = std::numeric_limits<real_t>::infinity();
			else data.at<real_t>(i, 0) = 2 * (data.at<real_t>(i, 0) - min.at<real_t>(i, 0)) / (max.at<real_t>(i, 0) - min.at<real_t>(i, 0)) - 1;
		}
	}

	inline void invMapminmax(cv::Mat &data, const cv::Mat &min, const cv::Mat &max) const {
		rep(i, data.rows) data.at<real_t>(i, 0) = (data.at<real_t>(i, 0) + 1) * (max.at<real_t>(i, 0) - min.at<real_t>(i, 0)) * 0.5 + min.at<real_t>(i, 0);
	}
};