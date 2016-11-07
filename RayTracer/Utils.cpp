#include "Utils.h"

#include <Windows.h>

size_t getConsoleWidth() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

void printProgress(real_t p) {
	size_t consoleWidth = getConsoleWidth();
	std::string prefix = "Rendering ... => ";
	size_t progressLen = (getConsoleWidth() - prefix.size() - 2) / 2;
	int f = (int) (p * progressLen);
	std::ostringstream oss;
	rep(i, f) oss << "\u25A0";
	rep(i, progressLen - f) oss << "  ";
	std::cout << "\r" << prefix << "[" << oss.str() << "]";
}