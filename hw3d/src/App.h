#pragma once
#include "Box.h"
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
	int run();
private:
	void render_frame();
private:
	const window window_;
#if defined(DEBUG) || defined(_DEBUG)
	const console console_;
#endif
	timer timer_;
	std::vector<std::unique_ptr<box>> boxes_;
};
