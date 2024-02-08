#include "AtumException.h"
#include <sstream>

atum_exception::atum_exception(const int line, const char* file) noexcept
	:
	line_(line),
	file_(file)
{}

const char* atum_exception::what() const noexcept
{
	std::ostringstream out{};
	out << get_type() << "\n"
		<< get_origin_string();
	what_buffer_ = out.str();
	return what_buffer_.c_str();
}

const char* atum_exception::get_type() const noexcept
{
	return "AtumException";
}

int atum_exception::get_line() const noexcept
{
	return line_;
}

const std::string& atum_exception::get_file() const noexcept
{
	return file_;
}

std::string atum_exception::get_origin_string() const noexcept
{
	std::ostringstream out{};
	out << "[File] " << file_ << "\n"
		<< "[Line] " << line_;
	return out.str();
}
