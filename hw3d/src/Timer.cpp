#include "Timer.h"

timer::timer() noexcept
{
	last_ = std::chrono::steady_clock::now();
}

float timer::mark() noexcept
{
	const auto old = last_;
	last_ = std::chrono::steady_clock::now();
	const std::chrono::duration<float> frame_time = last_ - old;
	return frame_time.count();
}

float timer::peek() const noexcept
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - last_).count();
}
