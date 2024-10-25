///////////////////////////////////////////////////////////////////////////////
// Filename: fps_class.cpp
///////////////////////////////////////////////////////////////////////////////
#include "fps_class.h"

void fps_class::initialize()
{
	this->fps_ = 0;
	this->count_ = 0;
	this->start_time_ = timeGetTime();
}

void fps_class::frame()
{
	this->count_++;

	if (timeGetTime() >= (this->start_time_ + 1000))
	{
		this->fps_ = this->count_;
		this->count_ = 0;

		this->start_time_ = timeGetTime();
	}
}

int fps_class::get_fps() const
{
	return this->fps_;
}
