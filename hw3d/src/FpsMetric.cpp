///////////////////////////////////////////////////////////////////////////////
// Filename: FpsMetric.cpp
///////////////////////////////////////////////////////////////////////////////
#include "FpsMetric.h"
#include <mmsystem.h>

void fps_metric::initialize()
{
	this->fps_ = 0;
	this->count_ = 0;
	this->start_time_ = timeGetTime();
}

void fps_metric::frame()
{
	this->count_++;

	if (timeGetTime() >= (this->start_time_ + 1000))
	{
		this->fps_ = this->count_;
		this->count_ = 0;

		this->start_time_ = timeGetTime();
	}
}

int fps_metric::get_fps() const
{
	return this->fps_;
}
