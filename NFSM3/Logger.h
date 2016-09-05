#pragma once

#include <string>

class Logger {
public:
	virtual Logger& operator<<(std::wstring) = 0;
	virtual Logger& operator<<(std::string) = 0;
	virtual Logger& operator<<(int) = 0;
	virtual ~Logger() {};
};