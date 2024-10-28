#pragma once
#include "Box.h"
#include "Window.h"
#include "Console.h"
#include "Timer.h"
#include "CpuMetric.h"
#include "wikibooks.org/fps_class.h"

class app
{
public:
	app();
	~app() = default;
	app(const app&) = delete;
	app& operator=(const app&) = delete;
	app(const app&&) = delete;
	app& operator=(const app&&) = delete;

	[[nodiscard]] int init();
	int run();

private:
	void render_frame();

private:
	const window window_;
#if defined(DEBUG) || defined(_DEBUG)
	const console console_;
	fps_class fps_{};
	cpu_metric cpu_{};
#endif
	timer timer_;
	std::vector<std::unique_ptr<class drawable>> drawables_;
	static constexpr size_t number_of_drawables_ = 180;
};
