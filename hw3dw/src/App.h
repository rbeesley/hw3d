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
	class exception : public atum_exception {
		using atum_exception::atum_exception;
	};
public:
	class app_exception : public exception
	{
	public:
		app_exception(int line, const char* file, const std::string& msg) noexcept;
		const char* get_type() const noexcept override;
	};

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
	mouse* p_mouse_;
	keyboard* p_keyboard_;
	graphics* p_graphics_;
	fps_metric fps_{};
	cpu_metric cpu_{};
#endif
	timer timer_;
	std::unique_ptr<gdi_plus_manager> p_gdi_manager_;
	std::vector<std::unique_ptr<class drawable>> drawables_;
	size_t number_of_drawables_ = 180;
};
