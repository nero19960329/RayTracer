#pragma once

#include "Utils.h"

#include <string>
#include <Windows.h>

class ProgressPrinter {
private:
	int nowCnt;
	real_t divAllCnt;
	std::string prefix;

	size_t progressLen;
	int boxCnt;
	std::wstring progressBar;

public:
	ProgressPrinter(const std::string &_prefix, int _allCnt) :
		nowCnt(0), divAllCnt(1.0 / _allCnt), prefix(_prefix), boxCnt(0) {
		progressLen = (getConsoleWidth() - prefix.size() - 2) / 2;

		progressBar.push_back('[');
		rep(i, progressLen) progressBar.push_back(0x3000);	// add spaces
		progressBar.push_back(']');
	}
	~ProgressPrinter() {}

	void display(int delta) {
		nowCnt += delta;

		int nowBoxCnt = (int) (nowCnt * progressLen * divAllCnt);
		if (nowBoxCnt - boxCnt < 1) return;

		repa(i, boxCnt, nowBoxCnt) progressBar[i + 1] = 0x25A0;	// add boxes
		boxCnt = nowBoxCnt;
		
		std::cout << "\r" << prefix;
		std::wcout.imbue(std::locale("chs"));
		std::wcout << progressBar;
	}

private:
	inline size_t getConsoleWidth() {
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		return csbi.srWindow.Right - csbi.srWindow.Left + 1;
	}
};