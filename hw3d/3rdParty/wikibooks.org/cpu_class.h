///////////////////////////////////////////////////////////////////////////////
// Filename: cpu_class.h
///////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "pdh.lib")

//////////////
// INCLUDES //
//////////////
#include <pdh.h>

///////////////////////////////////////////////////////////////////////////////
// Class name: cpu_class
///////////////////////////////////////////////////////////////////////////////
class cpu_class
{
public:
	cpu_class() = default;
	~cpu_class() = default;
	cpu_class(const cpu_class&) = delete;
	cpu_class& operator=(const cpu_class&) = delete;
	cpu_class(const cpu_class&&) = delete;
	cpu_class& operator=(const cpu_class&&) = delete;

	void initialize();
	void shutdown() const;
	void frame();
	[[nodiscard]] int get_cpu_percentage() const;

private:
	bool can_read_cpu_;
	HQUERY query_handle_;
	HCOUNTER counter_handle_;
	unsigned long last_sample_time_;
	long cpu_usage_;
};
