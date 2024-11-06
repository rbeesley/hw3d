#include "App.h"
#include "Box.h"

#include <DirectXMath.h>
#include <random>

#include "AtumMath.h"
#include "GDIPlusManager.h"
#include "Logging.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include "Surface.h"

class gdi_plus_manager gdi_plus_manager;

// 1280x720
// 800x600
constexpr int width = 1280;
constexpr int height = 720;
constexpr float aspect_ratio = static_cast<float>(height) / static_cast<float>(width);

app::app()
{
	PLOGI << "Initializing App";
	p_window_ = std::make_unique<window>();
#if (IS_DEBUG)
	p_console_ = std::make_unique<console>();
#endif
}

#if (IS_DEBUG)
static std::wstring exe_path() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(nullptr, buffer, MAX_PATH);
	const std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}
#endif

int app::initialize()
{
#if (IS_DEBUG)
	const auto console = p_console_.get();
	console->initialize(TEXT("Debug Console"));

	// Check Logging
	// This is the earliest this can be done if utilizing the console logger.
	PLOGN << "CWD: " << exe_path();
	PLOG_NONE << "";
	PLOG_NONE << "This is a NONE message";
	PLOG_FATAL << "This is a FATAL message";
	PLOG_ERROR << "This is an ERROR message";
	PLOG_WARNING << "This is a WARNING message";
	PLOG_INFO << "This is an INFO message";
	PLOG_DEBUG << "This is a DEBUG message";
	PLOG_VERBOSE << "This is a VERBOSE message";
	PLOG_NONE << "";

	// If the console failed to load, we want to make sure this is the last log message so that it is easier to spot.
	if (!console->get_handle())
	{
		PLOGE << "Failed to create Debug Console";
		return -3;
	}

	// Initialize the FPS Counter and CPU Counter
	fps_.initialize();
	cpu_.initialize();
#endif

	const auto window = p_window_.get();
	window->initialize(width, height, TEXT("Atum D3D Window"));
	if (!window->get_handle())
	{
		PLOGF << "Failed to create Window";
		return -2;
	}

	const auto rng_seed = std::random_device{}();
	PLOGI << "mt19937 rng seed: " << rng_seed;

#if true // if set to false, don't use the factory and draw a single drawable
	class drawable_factory
	{
	public:
		drawable_factory(graphics& graphics, const unsigned int rng_seed)
			:
			graphics_{ graphics },
			rng_{ rng_seed }
		{}

		int count = 0;
		std::unique_ptr<drawable> operator()()
		{
			switch (drawable_type_distribution_(rng_))
			{
			case 0:
				LOGV << "Drawable <pyramid>     #" << ++count;
				return std::make_unique<pyramid>(
					graphics_, rng_, distance_distribution_, spherical_coordinate_position_distribution_, rotation_of_drawable_distribution_,
					spherical_coordinate_movement_of_drawable_distribution_
				);
			case 1:
				LOGV << "Drawable <box>         #" << ++count;
				return std::make_unique<box>(
					graphics_, rng_, distance_distribution_, spherical_coordinate_position_distribution_, rotation_of_drawable_distribution_,
					spherical_coordinate_movement_of_drawable_distribution_, z_axis_distortion_distribution_
				);
			case 2:
				LOGV << "Drawable <melon>       #" << ++count;
				return std::make_unique<melon>(
					graphics_, rng_, distance_distribution_, spherical_coordinate_position_distribution_, rotation_of_drawable_distribution_,
					spherical_coordinate_movement_of_drawable_distribution_, latitude_distribution_, longitude_distribution_
				);
			case 3:
				LOGV << "Drawable <sheet>       #" << ++count;
				return std::make_unique<sheet>(
					graphics_, rng_, distance_distribution_, spherical_coordinate_position_distribution_, rotation_of_drawable_distribution_, spherical_coordinate_movement_of_drawable_distribution_
				);
			case 4:
				LOGV << "Drawable <skinned_box> #" << ++count;
				return std::make_unique<skinned_box>(
					graphics_, rng_, distance_distribution_, spherical_coordinate_position_distribution_, rotation_of_drawable_distribution_,
					spherical_coordinate_movement_of_drawable_distribution_
				);
				default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}	private:
			graphics& graphics_;
			std::mt19937 rng_;
			std::uniform_real_distribution<float> spherical_coordinate_position_distribution_{ 0.0f, PI * 2.0f };				// adist
			std::uniform_real_distribution<float> rotation_of_drawable_distribution_{ 0.0f, PI * 0.5f };						// ddist
			std::uniform_real_distribution<float> spherical_coordinate_movement_of_drawable_distribution_{ 0.0f, PI * 0.08f };	// odist
			std::uniform_real_distribution<float> distance_distribution_{ 6.0f, 20.0f };										// rdist
			std::uniform_real_distribution<float> z_axis_distortion_distribution_{ 0.4f, 3.0f };								// bdist
			std::uniform_int_distribution<int> latitude_distribution_{ 5, 20 };												// latdist
			std::uniform_int_distribution<int> longitude_distribution_{ 10, 40 };												// longdist
			std::uniform_int_distribution<int> drawable_type_distribution_{ 0, 4 };											// typedist
	};

	drawables_.reserve(number_of_drawables_);

	PLOGD << "Populating pool of drawables";
	std::generate_n(std::back_inserter(drawables_), number_of_drawables_, drawable_factory(window->get_graphics(), rng_seed));
#else
	{
		std::mt19937 rng{ rng_seed };
		std::uniform_real_distribution<float> distance_distribution{ 0.0f, 0.0f };
		//std::uniform_real_distribution<float> spherical_coordinate_position_distribution{ 0.0f, PI * 2.0f };
		std::uniform_real_distribution<float> spherical_coordinate_position_distribution{ 0.0f, 0.0f };
		std::uniform_real_distribution<float> rotation_of_drawable_distribution{ 0.0f, PI * 0.5f };
		//std::uniform_real_distribution<float> rotation_of_drawable_distribution{ 0.0f, 0.0f };
		//std::uniform_real_distribution<float> spherical_coordinate_movement_of_drawable_distribution{ 0.0f, PI * 0.08f };
		std::uniform_real_distribution<float> spherical_coordinate_movement_of_drawable_distribution{ 0.0f, 0.0f };
		drawables_.emplace_back(
			std::make_unique<skinned_box>(
				window->get_graphics(), rng, distance_distribution, spherical_coordinate_position_distribution, rotation_of_drawable_distribution,
				spherical_coordinate_movement_of_drawable_distribution
			));
	}
#endif

	PLOGD << "Set graphics projection";
	window->get_graphics().set_projection(
		DirectX::XMMatrixPerspectiveLH(
			1.0f,
			aspect_ratio,
			0.5f,
			40.0f));

	return 0;
}

#if (IS_DEBUG)
static std::wstring compose_fps_cpu_window_title(const int fps, const double cpu_percentage) {
	wchar_t buffer[50];
	std::swprintf(buffer, 50, L"fps: %d / cpu: %00.2f%%", fps, cpu_percentage);
	return std::wstring(buffer);
}
#endif

int app::run()
{
	const auto window = p_window_.get();

	PLOGI << "Starting Message Pump and Render Loop";
	while(true)
	{
		// Process pending messages without blocking
		if (const std::optional<int> exit_code = window->process_messages(); exit_code.has_value())
		{
			// If the optional return has a value, it is the exit code
			return *exit_code;
		}

#if (IS_DEBUG)
		fps_.frame();
		cpu_.frame();
		p_window_->set_title(compose_fps_cpu_window_title(fps_.get_fps(), cpu_.get_cpu_percentage()));
#endif
		render_frame();
	}
}

void app::shutdown() const
{
	PLOGI << "Shutdown App";
#if (IS_DEBUG)
	p_console_->shutdown();
#endif
	p_window_->shutdown();
}

void app::render_frame()
{
	const auto dt = timer_.mark();
	const auto window = p_window_.get();
	static keyboard& keyboard = window->get_keyboard();
	static graphics& graphics = window->get_graphics();

	graphics.clear_buffer(0.07f, 0.0f, 0.12f);
	for(const auto& drawable : drawables_)
	{
		drawable->update(keyboard.is_key_pressed(VK_SPACE) ? 0.0f : dt);
		drawable->draw(graphics);
	}

	graphics.end_frame();
}
