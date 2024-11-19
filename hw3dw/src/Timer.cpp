#include "Timer.h"

Timer::Timer() noexcept
{
	last_ = std::chrono::steady_clock::now();
}

float Timer::mark() noexcept
{
	const auto old = last_;
	last_ = std::chrono::steady_clock::now();
	const std::chrono::duration<float> frame_time = last_ - old;
	return frame_time.count();
}

float Timer::peek() const noexcept
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - last_).count();
}
