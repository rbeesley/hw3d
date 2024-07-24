#pragma once
#include <chrono>

class timer
{
public:
	timer();
	float mark();
	float peek() const;
private:
	std::chrono::steady_clock::time_point last;
};

