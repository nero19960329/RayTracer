#pragma once

#include "Utils.h"
#include "Timer.h"

#include <iomanip>
#include <string>
#include <sstream>
#include <Windows.h>

class ProgressPrinter {
private:
	Timer timer;

	long long nowCnt;
	long long allCnt;
	std::string prefix, suffix;

	size_t progressLen;
	size_t boxCnt;
	std::wstring progressBar;

public:
	ProgressPrinter(const std::string &_prefix, long long _allCnt) :
		nowCnt(0), allCnt(_allCnt), prefix(_prefix), boxCnt(0) {
		suffix = "Remaining: ";
		progressLen = (getConsoleWidth() - prefix.size() - suffix.size() - 12) / 2;

		progressBar.push_back('[');
		rep(i, progressLen) progressBar.push_back(0x3000);	// add spaces
		progressBar.push_back(']');

		timer.begin();
	}
	~ProgressPrinter() {}

	void start() {
		std::cout << "\r" << prefix;
		std::wcout.imbue(std::locale("chs"));
		std::wcout << progressBar;
		std::cout << suffix;
	}

	void display(int delta) {
		nowCnt += delta;

		size_t nowBoxCnt = (size_t) (nowCnt * progressLen * 1.0 / allCnt);
		if (nowBoxCnt - boxCnt < 1) return;

		repa(i, boxCnt, nowBoxCnt) progressBar[i + 1] = 0x25A0;	// add boxes
		boxCnt = nowBoxCnt;

		real_t nowDuration = timer.getDuration();
		int remainDuration = (int) (nowDuration * (allCnt - nowCnt) / nowCnt);
		
		std::cout << "\r" << prefix;
		std::wcout.imbue(std::locale("chs"));
		std::wcout << progressBar;
		std::cout << suffix;

		std::ostringstream oss;
		if (remainDuration >= 3600) {
			int hours = remainDuration / 3600;
			remainDuration -= hours * 3600;
			oss << hours << "h" << remainDuration / 60 << "m" << remainDuration % 60 << "s";
		} else if (remainDuration >= 60) {
			oss << remainDuration / 60 << "m" << remainDuration % 60 << "s";
		} else {
			oss << remainDuration << "s";
		}
		rep(i, 9 - oss.str().size()) std::cout << " ";
		std::cout << oss.str();
	}

	void finish() {
		repa(i, boxCnt, progressLen) progressBar[i + 1] = 0x25A0;
		std::cout << "\r" << prefix;
		std::wcout.imbue(std::locale("chs"));
		std::wcout << progressBar << std::endl;
	}

private:
	inline size_t getConsoleWidth() {
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		return csbi.srWindow.Right - csbi.srWindow.Left + 1;
	}
};