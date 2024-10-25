///////////////////////////////////////////////////////////////////////////////
// Filename: timer_class.cpp
///////////////////////////////////////////////////////////////////////////////
#include "timer_class.h"

bool timer_class::initialize()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&frequency_));
	if (frequency_ == 0)
	{
		return false;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	ticks_per_ms_ = static_cast<float>(frequency_) / 1000.0f;

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&start_time_));

	return true;
}

void timer_class::frame()
{
	INT64 current_time = 0;

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&current_time));

	const float time_difference = static_cast<float>(current_time - start_time_);

	frame_time_ = time_difference / ticks_per_ms_;

	start_time_ = current_time;

	return;
}

float timer_class::get_time() const
{
	return frame_time_;
}
