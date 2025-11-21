#include "AtumException.hpp"

#include <sstream>

AtumException::AtumException(const int line, const char* file) noexcept
	:
	line_(line),
	file_(file)
{}

const char* AtumException::what() const noexcept
{
	std::ostringstream out{};
	out << getType() << "\n"
		<< getOriginString();
	whatBuffer_ = out.str();
	return whatBuffer_.c_str();
}

const char* AtumException::getType() const noexcept
{
	return "AtumException";
}

int AtumException::getLine() const noexcept
{
	return line_;
}

const std::string& AtumException::getFile() const noexcept
{
	return file_;
}

std::string AtumException::getOriginString() const noexcept
{
	std::ostringstream out{};
	out << "[File] " << file_ << "\n"
		<< "[Line] " << line_;
	return out.str();
}
