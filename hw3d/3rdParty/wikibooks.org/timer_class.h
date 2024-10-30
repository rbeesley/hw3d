////////////////////////////////////////////////////////////////////////////////
// Filename: timer_class.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include <windows.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: timer_class
////////////////////////////////////////////////////////////////////////////////
class timer_class
{
public:
	timer_class() = default;
	~timer_class() = default;
	timer_class(const timer_class&) = delete;
	timer_class& operator=(const timer_class&) = delete;
	timer_class(const timer_class&&) = delete;
	timer_class& operator=(const timer_class&&) = delete;

	bool initialize();
	void frame();

	float get_time() const;

private:
	INT64 frequency_;
	float ticks_per_ms_;
	INT64 start_time_;
	float frame_time_;
};
