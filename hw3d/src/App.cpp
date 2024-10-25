#include "App.h"
#include "Box.h"

#include <DirectXMath.h>
#include <random>

#include "AtumMath.h"
#include "Logging.h"

// 1280x720
// 800x600
constexpr int width = 800;
constexpr int height = 600;
constexpr float aspect_ratio = static_cast<float>(height) / static_cast<float>(width);

app::app()
	: window_(width, height, TEXT("Atum D3D Window"))
#if defined(DEBUG) || defined(_DEBUG)
	, console_(TEXT("Debug Console"))
#endif
{
	PLOGI << "Initializing App";
	auto seed = std::random_device{}();

	PLOGD << "mt19937 seed: " << seed;
	std::mt19937 rng(seed);
	std::uniform_real_distribution<float> distance(6.0f, 20.0f);									// rdist
	std::uniform_real_distribution<float> spherical_coordinate_position(0.0f, TWOPI);				// adist
	std::uniform_real_distribution<float> rotation_of_box(0.0f, PI);								// ddist
	std::uniform_real_distribution<float> spherical_coordinate_movement_of_box(0.0f, PI * 0.08f);	// odist

	PLOGI << "Populating pool of drawables";
	for(auto i = 0; i < 180; i++)
	{
		boxes_.push_back(std::make_unique<box>(window::get_graphics(), rng, distance, spherical_coordinate_position, rotation_of_box, spherical_coordinate_movement_of_box));
	}

	PLOGI << "Set graphics projection";
	window::get_graphics().set_projection(
		DirectX::XMMatrixPerspectiveLH(
			1.0f, 
			aspect_ratio, 
			0.5f, 
			40.0f));
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

int app::run()
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

void app::render_frame()
{
	auto dt = timer_.mark();

	window::get_graphics().clear_buffer(0.07f, 0.0f, 0.12f);
	for(auto& box : boxes_)
	{
		box->update(dt);
		box->draw(window::get_graphics());
	}

	window::get_graphics().end_frame();
}
