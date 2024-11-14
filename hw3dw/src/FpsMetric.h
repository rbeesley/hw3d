#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: FpsMetric.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "winmm.lib")

//////////////
// INCLUDES //
//////////////
#define FPS_METRIC_
#include "AtumWindows.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: fps_class
////////////////////////////////////////////////////////////////////////////////
class fps_metric
{
public:
	fps_metric() = default;
	~fps_metric() = default;
	fps_metric(const fps_metric&) = delete;
	fps_metric& operator=(const fps_metric&) = delete;
	fps_metric(const fps_metric&&) = delete;
	fps_metric& operator=(const fps_metric&&) = delete;

	void initialize();
	void frame();
	[[nodiscard]] int get_fps() const;

private:
	int fps_, count_;
	unsigned long start_time_;
};
