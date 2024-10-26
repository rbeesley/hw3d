///////////////////////////////////////////////////////////////////////////////
// Filename: cpu_class.cpp
///////////////////////////////////////////////////////////////////////////////
#include "cpu_class.h"

void cpu_class::initialize()
{
	// Initialize the flag indicating whether this object can read the system cpu usage or not.
	this->can_read_cpu_ = true;

	// Create a query object to poll cpu usage.
	PDH_STATUS status = PdhOpenQuery(nullptr, 0, &this->query_handle_);
	if (status != ERROR_SUCCESS)
	{
		this->can_read_cpu_ = false;
	}

	// Set query object to poll all cpus in the system.
	status = PdhAddCounter(this->query_handle_, TEXT("\\Processor(_Total)\\% processor time"), 0, &this->counter_handle_);
	if (status != ERROR_SUCCESS)
	{
		this->can_read_cpu_ = false;
	}

	this->last_sample_time_ = GetTickCount64();

	this->cpu_usage_ = 0;
}


void cpu_class::shutdown() const
{
	if (this->can_read_cpu_)
	{
		PdhCloseQuery(this->query_handle_);
	}
}


void cpu_class::frame()
{
	PDH_FMT_COUNTERVALUE value;

	if (this->can_read_cpu_)
	{
		if ((this->last_sample_time_ + 1000) < GetTickCount64())
		{
			this->last_sample_time_ = GetTickCount64();

			PdhCollectQueryData(this->query_handle_);

			PdhGetFormattedCounterValue(this->counter_handle_, PDH_FMT_LONG, nullptr, &value);

			this->cpu_usage_ = value.longValue;
		}
	}
}


int cpu_class::get_cpu_percentage() const
{
	int usage;

	if (this->can_read_cpu_)
	{
		usage = static_cast<int>(this->cpu_usage_);
	}
	else
	{
		usage = 0;
	}

	return usage;
}
