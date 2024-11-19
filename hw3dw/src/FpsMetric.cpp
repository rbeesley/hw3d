#include "FpsMetric.h"
#include <mmsystem.h>

void FpsMetric::initialize()
{
	fps_ = 0;
	count_ = 0;
	startTime_ = timeGetTime();
}

void FpsMetric::frame()
{
	count_++;

	if (timeGetTime() >= (startTime_ + 1000))
	{
		fps_ = count_;
		count_ = 0;

		startTime_ = startTime_ + 1000;
	}
}

int FpsMetric::getFps() const
{
	return fps_;
}
