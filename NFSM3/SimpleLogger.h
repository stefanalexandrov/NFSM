#pragma once

#include "Logger.h"
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>

class SimpleLogger : public Logger {
public:
	static SimpleLogger& GetInstance() {
		static SimpleLogger instance;
		return instance;
	}
	Logger& operator<<(std::wstring);
	Logger& operator<<(std::string);
	Logger& operator<<(int);
	~SimpleLogger();
private:
	SimpleLogger();
	void openIfUnopened();
	std::ofstream m_log_file;
};
