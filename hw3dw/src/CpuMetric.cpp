#include "CpuMetric.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// Mostly based on code found in this stackoverflow post: https://stackoverflow.com/a/64166
// CPU percentage matches Task Manager and Process Explorer measurements, but is significantly more than System Informer

void cpu_metric::initialize() {
	SYSTEM_INFO sys_info;
	FILETIME fcreate, fexit, fsys, fuser;

	GetSystemInfo(&sys_info);
	number_of_processors_ = static_cast<int>(sys_info.dwNumberOfProcessors);

	GetSystemTimeAsFileTime(&fexit);
	memcpy(&last_cpu_, &fexit, sizeof(FILETIME));

	self_ = GetCurrentProcess();
	GetProcessTimes(self_, &fcreate, &fexit, &fsys, &fuser);
	memcpy(&last_sys_cpu_, &fsys, sizeof(FILETIME));
	memcpy(&last_user_cpu_, &fuser, sizeof(FILETIME));
	start_time_ = timeGetTime();
}

void cpu_metric::frame()
{
	FILETIME fcreate, fexit, fsys, fuser;
	ULARGE_INTEGER now, sys, user;

	// Update once a second
	if (timeGetTime() >= (start_time_ + 1000))
	{
		GetSystemTimeAsFileTime(&fexit);
		memcpy(&now, &fexit, sizeof(FILETIME));

		GetProcessTimes(self_, &fcreate, &fexit, &fsys, &fuser);
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
		start_time_ = start_time_ + 1000;
	}
}

double cpu_metric::get_cpu_percentage() const
{
	return cpu_usage_;
}
