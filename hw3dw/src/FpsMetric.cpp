///////////////////////////////////////////////////////////////////////////////
// Filename: FpsMetric.cpp
///////////////////////////////////////////////////////////////////////////////
#include "FpsMetric.h"
#include <mmsystem.h>

void fps_metric::initialize()
{
	fps_ = 0;
	count_ = 0;
	start_time_ = timeGetTime();
}

void fps_metric::frame()
{
	count_++;

	if (timeGetTime() >= (start_time_ + 1000))
	{
		fps_ = count_;
		count_ = 0;

		start_time_ = start_time_ + 1000;
	}
}

int fps_metric::get_fps() const
{
	return fps_;
}
