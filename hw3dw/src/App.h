#pragma once
#include "Box.h"
#include "Window.h"
#include "Console.h"
#include "Timer.h"
#include "CpuMetric.h"
#include "FpsMetric.h"
#include "GDIPlusManager.h"

class app
{
public:
	app();
	~app() = default;
	app(const app&) = delete;
	app& operator=(const app&) = delete;
	app(const app&&) = delete;
	app& operator=(const app&&) = delete;

	[[nodiscard]] int initialize();
	int run();
	void shutdown() const;

private:
	void render_frame(const ImVec4& clear_color);

private:
	std::unique_ptr<window> p_window_;
#if (IS_DEBUG)
	std::unique_ptr<console> p_console_;
	fps_metric fps_{};
	cpu_metric cpu_{};
#endif
	timer timer_;
	std::unique_ptr<gdi_plus_manager> p_gdi_manager_;
	std::vector<std::unique_ptr<class drawable>> drawables_;
	static constexpr size_t number_of_drawables_ = 180;
};
