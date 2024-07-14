#pragma once
#include "Window.h"
#include "Console.h"

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
	int run();
private:
	void process_frame();
private:
	const window window_;
#if defined(_DEBUG)
	const console console_;
#endif
};
