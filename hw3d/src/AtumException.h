#pragma once
#include <exception>
#include <string>

class atum_exception : public std::exception {
public:
	atum_exception(int line, const char* file) noexcept;
	atum_exception() = delete;

	const char* what() const noexcept override;
	virtual const char* get_type() const noexcept;
	int get_line() const noexcept;
	const std::string& get_file() const noexcept;
	std::string get_origin_string() const noexcept;
private:
	int line_;
	std::string file_;
protected:
	mutable std::string what_buffer_;
};
