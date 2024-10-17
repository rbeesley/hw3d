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

void app::render_frame() const
{
	const float c = sin(timer_.peek()) / 2.f + 0.5f;
	window_.get_graphics().clear_buffer(c, c, 1.f);

	// Experiment with drawing graphics
	window_.get_graphics().draw_test_triangle();

	window_.get_graphics().end_frame();
}
