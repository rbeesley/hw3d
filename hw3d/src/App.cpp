#include "App.h"
#include "Logging.h"

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

inline float map(const float in, const float in_min, const float in_max, const float out_min, const float out_max)
{
	return (in - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline float map(const int in, const int in_min, const int in_max, const int out_min, const int out_max)
{
	return map(static_cast<float>(in), static_cast<float>(in_min), static_cast<float>(in_max), static_cast<float>(out_min), static_cast<float>(out_max));
}

void app::render_frame() const
{
	const float c = sin(timer_.peek()) / 2.0f + 0.5f;
	window::graphics()->clear_buffer(c, c, 1.0f);

	// Experiment with drawing graphics
	window::graphics()->draw_test_triangle(
		timer_.peek(),
		map(window::mouse()->pos().x, 0, width-1, -1, 1),
		map(window::mouse()->pos().y, 0, height-1, 1, -1)
	);

	window::graphics()->end_frame();
}
