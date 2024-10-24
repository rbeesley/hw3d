#include "App.h"
#include "Box.h"

#include <DirectXMath.h>
#include <random>

#include "Logging.h"

// 1280x720
// 800x600
constexpr int width = 800;
constexpr int height = 600;
constexpr float aspect_ratio = static_cast<float>(height) / static_cast<float>(width);

constexpr float PI = 3.141592654f;
constexpr float TWOPI = 6.283185307f;
constexpr float ONEDIVPI = 0.318309886f;
constexpr float ONEDIV2PI = 0.159154943f;
constexpr float PIDIV2 = 1.570796327f;
constexpr float PIDIV4 = 0.785398163f;

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
	std::uniform_real_distribution<float> distance(6.0f, 20.0f);
	std::uniform_real_distribution<float> rotation_of_box(0.0f, TWOPI);
	std::uniform_real_distribution<float> spherical_coordinate_position(0.0f, TWOPI);
	std::uniform_real_distribution<float> spherical_coordinate_movement_of_box(0.0f, PI * 0.3f);

	PLOGI << "Populating pool of drawables (box)";
	for(auto i = 0; i < 80; i++)
	{
		boxes_.push_back(std::make_unique<box>(window::get_graphics(), rng, distance, rotation_of_box, spherical_coordinate_position, spherical_coordinate_movement_of_box));
	}

	PLOGI << "Set graphics projection";
	window::get_graphics().set_projection(DirectX::XMMatrixPerspectiveLH(1.0f, aspect_ratio, 0.5f, 40.0f));
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

static float map(const float in, const float in_min, const float in_max, const float out_min, const float out_max)
{
	return (in - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

[[maybe_unused]] static float map(const int in, const int in_min, const int in_max, const int out_min, const int out_max)
{
	return map(static_cast<float>(in), static_cast<float>(in_min), static_cast<float>(in_max), static_cast<float>(out_min), static_cast<float>(out_max));
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
