#include "AtumException.h"
#include <sstream>

AtumException::AtumException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

const char* AtumException::what() const noexcept
{
	std::ostringstream out{};
	out << GetType() << "\n"
		<< GetOriginString();
	whatBuffer = out.str();
	return whatBuffer.c_str();
}

const char* AtumException::GetType() const noexcept
{
	return "AtumException";
}

int AtumException::GetLine() const noexcept
{
	return line;
}

const std::string& AtumException::GetFile() const noexcept
{
	return file;
}

std::string AtumException::GetOriginString() const noexcept
{
	std::ostringstream out{};
	out << "[File] " << file << "\n"
		<< "[Line] " << line;
	return out.str();
}
