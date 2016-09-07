#include "stdafx.h"
#include "SimpleLogger.h"


SimpleLogger::SimpleLogger() {
}
SimpleLogger::~SimpleLogger() {
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t tt = std::chrono::system_clock::to_time_t(now);
	m_log_file << "Stop logging " << ctime(&tt) << "\n";
	m_log_file.close();
}
Logger& SimpleLogger::operator<<(std::wstring st) {
	std::string tmp(st.begin(), st.end());
	openIfUnopened();
	m_log_file << tmp;
	m_log_file.flush();
	return *this;
}
Logger& SimpleLogger::operator<<(std::string st) {
	openIfUnopened();
	m_log_file << st;
	m_log_file.flush();
	return *this;
}
Logger& SimpleLogger::operator<<(int n) {
	m_log_file << n;
	m_log_file.flush();
	return *this;
}
void SimpleLogger::openIfUnopened() {
	if (!m_log_file.is_open()) {
		m_log_file.open("log.txt", std::ofstream::app);
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::time_t tt = std::chrono::system_clock::to_time_t(now);
		m_log_file << "Start logging>>>" << ctime(&tt) << "\n";
	}
}