#pragma once
#pragma comment(lib, "winmm.lib")
#define FPS_METRIC_
#include "AtumWindows.h"

class FpsMetric
{
public:
	FpsMetric() = default;
	~FpsMetric() = default;
	FpsMetric(const FpsMetric&) = delete;
	FpsMetric& operator=(const FpsMetric&) = delete;
	FpsMetric(const FpsMetric&&) = delete;
	FpsMetric& operator=(const FpsMetric&&) = delete;

	void initialize();
	void frame();
	[[nodiscard]] int getFps() const;

private:
	int fps_, count_;
	unsigned long startTime_;
};
