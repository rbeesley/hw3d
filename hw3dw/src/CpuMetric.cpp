#include "CpuMetric.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// Mostly based on code found in this stackoverflow post: https://stackoverflow.com/a/64166

void cpu_metric::initialize() {
	SYSTEM_INFO sys_info;
	FILETIME ftime, fsys, fuser;

	GetSystemInfo(&sys_info);
	number_of_processors_ = static_cast<int>(sys_info.dwNumberOfProcessors);

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&last_cpu_, &ftime, sizeof(FILETIME));

	self_ = GetCurrentProcess();
	GetProcessTimes(self_, &ftime, &ftime, &fsys, &fuser);
	memcpy(&last_sys_cpu_, &fsys, sizeof(FILETIME));
	memcpy(&last_user_cpu_, &fuser, sizeof(FILETIME));
	start_time_ = timeGetTime();
}


void cpu_metric::frame()
{
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;

	// Update once a second
	if (timeGetTime() >= (this->start_time_ + 1000))
	{
		GetSystemTimeAsFileTime(&ftime);
		memcpy(&now, &ftime, sizeof(FILETIME));

		GetProcessTimes(self_, &ftime, &ftime, &fsys, &fuser);
		memcpy(&sys, &fsys, sizeof(FILETIME));
		memcpy(&user, &fuser, sizeof(FILETIME));
		double percent = static_cast<double>(
			(sys.QuadPart - last_sys_cpu_.QuadPart) +
			(user.QuadPart - last_user_cpu_.QuadPart));
		percent /= static_cast<double>(now.QuadPart - last_cpu_.QuadPart);
		percent /= number_of_processors_;
		cpu_usage_ = percent * 100;
		last_cpu_ = now;
		last_user_cpu_ = user;
		last_sys_cpu_ = sys;
		start_time_ = timeGetTime();
	}
}

double cpu_metric::get_cpu_percentage() const
{
	return cpu_usage_;
}
