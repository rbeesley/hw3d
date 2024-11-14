#pragma once
#include <chrono>

class timer
{
public:
	timer() noexcept;

	~timer() = default;
	timer(const timer&) = delete;
	timer& operator=(const timer&) = delete;
	timer(const timer&&) = delete;
	timer& operator=(const timer&&) = delete;

	float mark() noexcept;
	float peek() const noexcept;
private:
	std::chrono::steady_clock::time_point last_;
};

