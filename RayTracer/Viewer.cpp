#include "Viewer.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

vector<vector<vector<Ray>>> Viewer::getRayVector(int sample) const {
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

	vector<vector<vector<Ray>>> res;
	Vec3 tmpLeft = LT - deltaH;
	if (sample > 1) {
		srand(unsigned(time(0)));
		rep(i, screen.width) {
			vector<vector<Ray>> row;
			Vec3 tmpVec = tmpLeft;
			rep(j, screen.height) {
				vector<Ray> col;
				rep(k, sample) {
					col.emplace_back(center, (tmpVec - rand() * deltaH / RAND_MAX + rand() * deltaW / RAND_MAX - center).getNormalized());
				}
				row.emplace_back(col);
				tmpVec -= deltaH;
			}
			tmpLeft += deltaW;
			res.emplace_back(row);
		}
	} else {
		rep(i, screen.width) {
			vector<vector<Ray>> row;
			Vec3 tmpVec = tmpLeft;
			rep(j, screen.height) {
				vector<Ray> col;
				col.emplace_back(center, (tmpVec - center).getNormalized());
				row.emplace_back(col);
				tmpVec -= deltaH;
			}
			tmpLeft += deltaW;
			res.emplace_back(row);
		}
	}

	return res;
}