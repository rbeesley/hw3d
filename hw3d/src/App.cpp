#include "App.h"
#include "Logging.h"

// 1280x720
// 800x600
constexpr int width = 800;
constexpr int height = 600;

app::app()
	: window_(width, height, TEXT("Atum D3D Window"))
#if defined(DEBUG) || defined(_DEBUG)
	, console_(TEXT("Debug Console"))
#endif
{
	PLOGI << "Initializing App";
}

int app::init() const
{
	if (!window_.get_handle())
	{
		PLOGF << "Failed to create Window";
		return -2;
	}

#if defined(DEBUG) || defined(_DEBUG)
	if (!console_.get_window_handle())
	{
		PLOGE << "Failed to create Debug Console";
		return -3;
	}
#endif

	return 0;
}

int app::run() const
{
	PLOGI << "Starting Message Pump and Render Loop";

	while(true)
	{
		// Process pending messages without blocking
		if (const auto exit_code = window::process_messages())
		{
			// If the optional return has a value, it is the exit code
			return *exit_code;
		}
		render_frame();
	}
}

static float map(const float in, const float in_min, const float in_max, const float out_min, const float out_max)
{
	return (in - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static float map(const int in, const int in_min, const int in_max, const int out_min, const int out_max)
{
	return map(static_cast<float>(in), static_cast<float>(in_min), static_cast<float>(in_max), static_cast<float>(out_min), static_cast<float>(out_max));
}

constexpr float PI = 3.141592654f;
constexpr float TWOPI = 6.283185307f;
constexpr float ONEDIVPI = 0.318309886f;
constexpr float ONEDIV2PI = 0.159154943f;
constexpr float PIDIV2 = 1.570796327f;
constexpr float PIDIV4 = 0.785398163f;

void app::render_frame() const
{
	const float c = map(sin(timer_.peek()), -1.0f, 1.0f, 0.25f, 0.75f);
	window::get_graphics()->clear_buffer(c, c, 1.0f);

	// Experiment with drawing graphics
	window::get_graphics()->draw_test_triangle(
		map(static_cast<float>(window::get_mouse()->pos().x), 0.0f, width - 1.0f, 0.0f, 5 * TWOPI) + timer_.peek(),
		0, 0, 0
	);
	window::get_graphics()->draw_test_triangle(
		-timer_.peek(),
		0, 0, map(window::get_mouse()->pos().y, 0, height - 1, 5, -1)
	);

	window::get_graphics()->end_frame();
}
