#pragma once
#include "AtumWindows.hpp"

class CpuMetric
{
public:
	CpuMetric() = default;
	~CpuMetric() = default;
	CpuMetric(const CpuMetric&) = delete;
	CpuMetric& operator=(const CpuMetric&) = delete;
	CpuMetric(const CpuMetric&&) = delete;
	CpuMetric& operator=(const CpuMetric&&) = delete;

	void initialize();
	void frame();
	[[nodiscard]] double getCpuPercentage() const;

private:
	unsigned long startTime_;
	ULARGE_INTEGER lastCpu_, lastSysCpu_, lastUserCpu_;
	int numberOfProcessors_;
	HANDLE currentProcess_;
	double cpuUsage_;
};
