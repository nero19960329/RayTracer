#pragma once

#include <mat.h>

#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

#include "Utils.h"

class MatReader {
	std::string filename;

public:
	MatReader(const std::string &_name) :
		filename(_name) {}

	std::vector<cv::Mat> readMats() const {
		MATFile *pmat;

		pmat = matOpen(filename.c_str(), "r");
		if (!pmat) error_exit("Open file error!\n");

		mxArray *pa = matGetVariable(pmat, "nnwb");
		if (!pa) error_exit("Null mat file!\n");

		std::vector<cv::Mat> res;

		mwSize num = mxGetNumberOfElements(pa);
		rep(i, num) {
			mxArray *parr = mxGetCell(pa, i);
			real_t *pr = mxGetPr(parr);
			if (!pr) error_exit("Null mat file!\n");
			res.push_back(getCVMat(pr, mxGetM(parr), mxGetN(parr)));
		}

		mxDestroyArray(pa);
		matClose(pmat);

		return res;
	}

private:
	cv::Mat getCVMat(real_t *p, int rows, int cols) const {
		cv::Mat res(rows, cols, CV_64F);
		rep(i, rows) rep(j, cols) res.at<real_t>(i, j) = p[j * rows + i];
		return std::move(res);
	}
};