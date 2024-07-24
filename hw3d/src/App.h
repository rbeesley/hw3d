#pragma once
#include "Window.h"
#include "Console.h"
#include "Timer.h"

class app
{
public:
	app();
	~app() = default;
	app(const app&) = delete;
	app& operator=(const app&) = delete;
	app(const app&&) = delete;
	app& operator=(const app&&) = delete;
	[[nodiscard]] int init() const;
	int run() const;
private:
	void process_frame() const;
private:
	const window window_;
#if defined(_DEBUG)
	const console console_;
#endif
	const timer timer_;
};
