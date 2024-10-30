///////////////////////////////////////////////////////////////////////////////
// Filename: cpu_class.h
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <windows.h>

class cpu_metric
{
public:
	cpu_metric() = default;
	~cpu_metric() = default;
	cpu_metric(const cpu_metric&) = delete;
	cpu_metric& operator=(const cpu_metric&) = delete;
	cpu_metric(const cpu_metric&&) = delete;
	cpu_metric& operator=(const cpu_metric&&) = delete;

	void initialize();
	void frame();
	[[nodiscard]] double get_cpu_percentage() const;

private:
	unsigned long start_time_;
	ULARGE_INTEGER last_cpu_, last_sys_cpu_, last_user_cpu_;
	int number_of_processors_;
	HANDLE self_;
	double cpu_usage_;
};
