#include "App.h"
#include "Box.h"

#include <DirectXMath.h>
#include <random>

#include "AtumMath.h"
#include "Logging.h"
#include "Melon.h"
#include "Pyramid.h"

// 1280x720
// 800x600
constexpr int width = 1280;
constexpr int height = 720;
constexpr float aspect_ratio = static_cast<float>(height) / static_cast<float>(width);

app::app()
{
	PLOGI << "Initializing App";
	p_window_ = std::make_unique<window>();
	p_console_ = std::make_unique<console>();
}

int app::initialize()
{
#if defined(DEBUG) || defined(_DEBUG)
	p_console_->initialize(TEXT("Debug Console"));
	if (!p_console_->get_handle())
	{
		PLOGE << "Failed to create Debug Console";
		return -3;
	}

	fps_.initialize();
	cpu_.initialize();
#endif

	p_window_->initialize(width, height, TEXT("Atum D3D Window"));
	if (!p_window_->get_handle())
	{
		PLOGF << "Failed to create Window";
		return -2;
	}

	const auto rng_seed = std::random_device{}();
	PLOGI << "mt19937 rng seed: " << rng_seed;

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
				LOGV << "Drawable <pyramid> #" << ++count;
				return std::make_unique<pyramid>(
					graphics_, rng_, distance_distribution_, spherical_coordinate_position_distribution_, rotation_of_drawable_distribution_,
					spherical_coordinate_movement_of_drawable_distribution_
				);
			case 1:
				LOGV << "Drawable <box>     #" << ++count;
				return std::make_unique<box>(
					graphics_, rng_, distance_distribution_, spherical_coordinate_position_distribution_, rotation_of_drawable_distribution_,
					spherical_coordinate_movement_of_drawable_distribution_, z_axis_distortion_distribution_
				);
			case 2:
				LOGV << "Drawable <melon>   #" << ++count;
				return std::make_unique<melon>(
					graphics_, rng_, distance_distribution_, spherical_coordinate_position_distribution_, rotation_of_drawable_distribution_,
					spherical_coordinate_movement_of_drawable_distribution_, latitude_distribution_, longitude_distribution_
				);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}	private:
			graphics& graphics_;
			std::mt19937 rng_;
			std::uniform_real_distribution<float> spherical_coordinate_position_distribution_{ 0.0f,PI * 2.0f };				// adist
			std::uniform_real_distribution<float> rotation_of_drawable_distribution_{ 0.0f,PI * 0.5f };						// ddist
			std::uniform_real_distribution<float> spherical_coordinate_movement_of_drawable_distribution_{ 0.0f,PI * 0.08f };	// odist
			std::uniform_real_distribution<float> distance_distribution_{ 6.0f,20.0f };										// rdist
			std::uniform_real_distribution<float> z_axis_distortion_distribution_{ 0.4f,3.0f };								// bdist
			std::uniform_int_distribution<int> latitude_distribution_{ 5,20 };													// latdist
			std::uniform_int_distribution<int> longitude_distribution_{ 10,40 };												// longdist
			std::uniform_int_distribution<int> drawable_type_distribution_{ 0,2 };												// typedist
	};

	const drawable_factory drawable_factory(p_window_->get_graphics(), rng_seed);
	drawables_.reserve(number_of_drawables_);

	PLOGD << "Populating pool of drawables";
	std::generate_n(std::back_inserter(drawables_), number_of_drawables_, drawable_factory);

	PLOGD << "Set graphics projection";
	p_window_->get_graphics().set_projection(
		DirectX::XMMatrixPerspectiveLH(
			1.0f,
			aspect_ratio,
			0.5f,
			40.0f));

	return 0;
}

#if defined(DEBUG) || defined(_DEBUG)
static std::wstring compose_fps_cpu_window_title(const int fps, const double cpu_percentage) {
	wchar_t buffer[50];
	std::swprintf(buffer, 50, L"fps: %d / cpu: %00.2f%%", fps, cpu_percentage);
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
		p_window_->set_title(compose_fps_cpu_window_title(fps_.get_fps(), cpu_.get_cpu_percentage()));
#endif
		render_frame();
	}
}

void app::shutdown() const
{
	PLOGI << "Shutdown App";
	p_console_->shutdown();
	p_window_->shutdown();
}

void app::render_frame()
{
	const auto dt = timer_.mark();

	window::get_graphics().clear_buffer(0.07f, 0.0f, 0.12f);
	for(const auto& drawable : drawables_)
	{
		drawable->update(dt);
		drawable->draw(window::get_graphics());
	}

	window::get_graphics().end_frame();
}
