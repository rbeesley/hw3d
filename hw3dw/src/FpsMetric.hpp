#pragma once
#pragma comment(lib, "winmm.lib")

#include "AtumWindows.hpp"

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
