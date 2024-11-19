#pragma once
#include <chrono>

class Timer
{
public:
	Timer() noexcept;

	~Timer() = default;
	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer(const Timer&&) = delete;
	Timer& operator=(const Timer&&) = delete;

	float mark() noexcept;
	float peek() const noexcept;
private:
	std::chrono::steady_clock::time_point last_;
};

