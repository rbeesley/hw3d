#include "Timer.h"

timer::timer() noexcept
{
	last = std::chrono::steady_clock::now();
}

float timer::mark() noexcept
{
	const auto old = last;
	last = std::chrono::steady_clock::now();
	const std::chrono::duration<float> frameTime = last - old;
	return frameTime.count();
}

float timer::peek() const noexcept
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count();
}
