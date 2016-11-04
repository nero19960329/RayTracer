#include "Viewer.h"
#include <iostream>

using namespace std;

vector<vector<Ray>> Viewer::getRayVector() const {
	real_t w, h;
	h = 2 * tan(fovy * PI / 360);
	w = h * screen.getRatio();
	Vec3 P = center + dir * 1.0;
	Vec3 x, y;
	x = dir.cross(up);
	y = up;
	Vec3 halfW, halfH;
	halfW = (w / 2) * x;
	halfH = (h / 2) * y;
	Vec3 LT, LB, RB, RT;
	LT = P - halfW + halfH;
	LB = P - halfW - halfH;
	RB = P + halfW - halfH;
	RT = P + halfW + halfH;
	Vec3 deltaW, deltaH;
	deltaW = (halfW + halfW) / screen.width;
	deltaH = (halfH + halfH) / screen.height;

	vector<vector<Ray>> res;
	Vec3 tmpLeft = LT - deltaH;
	rep(i, screen.width) {
		vector<Ray> row;
		Vec3 tmpVec = tmpLeft;
		rep(j, screen.height) {
			row.push_back({ center, (tmpVec - center).getNormalized() });
			tmpVec -= deltaH;
		}
		tmpLeft += deltaW;
		res.push_back(row);
	}

	return res;
}