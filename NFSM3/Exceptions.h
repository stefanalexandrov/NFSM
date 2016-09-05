#pragma once

#include <exception>
#include <string>

class Nullptr : public std::exception
{
public:
	Nullptr(const std::string& er) : m_error(er) {}
	virtual const char* what() const noexcept override
	{
		return m_error.c_str();
	}
private:
	std::string m_error;
};

class Badargument : public std::exception
{
public:
	Badargument(const std::string& er) : m_error(er) {}
	virtual const char* what() const noexcept override
	{
		return m_error.c_str();
	}
private:
	std::string m_error;
};
