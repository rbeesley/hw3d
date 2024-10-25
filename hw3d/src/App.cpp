#include "App.h"
#include "Box.h"

#include <DirectXMath.h>
#include <random>

#include "AtumMath.h"
#include "Logging.h"

// 1280x720
// 800x600
constexpr int width = 1280;
constexpr int height = 720;
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

app::~app()
{
#if defined(DEBUG) || defined(_DEBUG)
	cpu_.shutdown();
#endif
}

int app::init()
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

	fps_.initialize();
	cpu_.initialize();
#endif

	return 0;
}

#if defined(DEBUG) || defined(_DEBUG)
static std::wstring get_fps_cpu_window_title(const int fps, const int cpu_percentage) {
	wchar_t buffer[50];
	std::swprintf(buffer, 50, L"fps: %d / cpu: %d%%", fps, cpu_percentage);
	return std::wstring(buffer);
}
#endif

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
#if defined(DEBUG) || defined(_DEBUG)
		fps_.frame();
		cpu_.frame();
		window_.set_title(get_fps_cpu_window_title(fps_.get_fps(), cpu_.get_cpu_percentage()));
#endif
		render_frame();
	}
}

void app::render_frame()
{
	const auto dt = timer_.mark();

	window::get_graphics().clear_buffer(0.07f, 0.0f, 0.12f);
	for(const auto& box : boxes_)
	{
		box->update(dt);
		box->draw(window::get_graphics());
	}

	window::get_graphics().end_frame();
}
