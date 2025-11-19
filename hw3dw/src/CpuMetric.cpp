#include "CpuMetric.hpp"

#pragma comment(lib, "winmm.lib")

// Mostly based on code found in this stackoverflow post: https://stackoverflow.com/a/64166

void CpuMetric::initialize() {
	SYSTEM_INFO systemInfo;
	FILETIME fcreate, fexit, fsys, fuser;

	GetSystemInfo(&systemInfo);
	numberOfProcessors_ = static_cast<int>(systemInfo.dwNumberOfProcessors);

	GetSystemTimeAsFileTime(&fexit);
	memcpy(&lastCpu_, &fexit, sizeof(FILETIME));

	currentProcess_ = GetCurrentProcess();
	GetProcessTimes(currentProcess_, &fcreate, &fexit, &fsys, &fuser);
	memcpy(&lastSysCpu_, &fsys, sizeof(FILETIME));
	memcpy(&lastUserCpu_, &fuser, sizeof(FILETIME));
	startTime_ = timeGetTime();
}

void CpuMetric::frame()
{
	FILETIME fcreate, fexit, fsys, fuser;
	ULARGE_INTEGER now, sys, user;

	// Update once a second
	if (timeGetTime() >= (startTime_ + 1000))
	{
		GetSystemTimeAsFileTime(&fexit);
		memcpy(&now, &fexit, sizeof(FILETIME));

		GetProcessTimes(currentProcess_, &fcreate, &fexit, &fsys, &fuser);
		memcpy(&sys, &fsys, sizeof(FILETIME));
		memcpy(&user, &fuser, sizeof(FILETIME));
		double percent = static_cast<double>(
			(sys.QuadPart - lastSysCpu_.QuadPart) +
			(user.QuadPart - lastUserCpu_.QuadPart));
		percent /= static_cast<double>(now.QuadPart - lastCpu_.QuadPart);
		percent /= numberOfProcessors_;
		cpuUsage_ = percent * 100;
		lastCpu_ = now;
		lastUserCpu_ = user;
		lastSysCpu_ = sys;
		startTime_ = timeGetTime();
	}
}

double CpuMetric::getCpuPercentage() const
{
	return cpuUsage_;
}
