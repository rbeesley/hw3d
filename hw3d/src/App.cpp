#include <3rdParty/ImGui/imgui.h>
#include <DirectXMath.h>
#include <random>

#include "App.h"
#include "AtumMath.h"
#include "Box.h"
#include "GDIPlusManager.h"
#include "Logging.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include "Surface.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

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

int app::initialize()
{
#if (IS_DEBUG)
	const auto console = p_console_.get();
	console->initialize(TEXT("Debug Console"));

	auto exe_path = []
	{
		TCHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileName(nullptr, buffer, MAX_PATH);
		const std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
		return std::wstring(buffer).substr(0, pos);
	};

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
		PLOGD << "Draw a skinned_box";
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

int app::run()
{
	bool show_demo_window = true;
	bool show_another_window = false;
	auto clear_color = ImVec4(0.07f, 0.0f, 0.12f, 1.00f);
	const ImGuiIO& im_gui_io = ImGui::GetIO();

	const auto window = p_window_.get();

	PLOGI << "Starting Message Pump and Render Loop";
	constexpr bool done = false;
	while (!done)
	{
		//PLOGI << " *** Run loop ***";
		// Process pending messages without blocking
		if (const std::optional<unsigned int> exit_code = window->process_messages(); exit_code.has_value())
		{
			if (exit_code.value() == WM_QUIT)
			{
				return 0;
			}
		}

#if (IS_DEBUG)
		auto static compose_fps_cpu_window_title = [](const int fps, const double cpu_percentage) {
			wchar_t buffer[50];
			std::swprintf(buffer, 50, L"fps: %d / cpu: %00.2f%%", fps, cpu_percentage);
			return std::wstring(buffer);
		};

		fps_.frame();
		cpu_.frame();
		p_window_->set_title(compose_fps_cpu_window_title(fps_.get_fps(), cpu_.get_cpu_percentage()));
#endif

		PLOGV << "window->get_graphics().begin_frame()";
		if(auto [width, height] = window->get_target_dimensions(); !window->get_graphics().begin_frame(width, height))
		{
			if (width > 0 || height > 0)
			{
				window->set_target_dimensions(0, 0);
			}
			continue;
		}

		// Start the Dear ImGui frame
		PLOGD << "Start the Dear ImGui frame";
		PLOGV << "ImGui_ImplDX11_NewFrame()";
		ImGui_ImplDX11_NewFrame();
		PLOGV << "ImGui_ImplWin32_NewFrame()";
		ImGui_ImplWin32_NewFrame();
		PLOGV << "ImGui::NewFrame()";
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			PLOGD << "Show the big demo window";
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			PLOGD << "Show a simple Dear ImGui window";
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&clear_color)); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / im_gui_io.Framerate, im_gui_io.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			PLOGD << "Show another simple Dear ImGui window";
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		PLOGV << "ImGui::Render();";
		ImGui::Render();
		PLOGV << "render_frame(clear_color);";
		render_frame(clear_color);
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

void app::render_frame(const ImVec4& clear_color)
{
	const auto dt = timer_.mark();
	const auto window = p_window_.get();
	static keyboard& keyboard = window->get_keyboard();
	static graphics& graphics = window->get_graphics();

	PLOGD << "Fetch Dear ImGui IO";
	const ImGuiIO& im_gui_io = ImGui::GetIO(); (void)im_gui_io;

	PLOGD << "Clear the buffer";
	graphics.clear_buffer(clear_color);

	PLOGD << "Draw all drawables";
	for (const auto& drawable : drawables_)
	{
		drawable->update(keyboard.is_key_pressed(VK_SPACE) ? 0.0f : dt);
		drawable->draw(graphics);
	}

	PLOGV << "ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData())";
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

#ifdef IMGUI_DOCKING
	// Update and Render additional Platform Windows
	if (im_gui_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		PLOGV << "ImGui::UpdatePlatformWindows()";
		ImGui::UpdatePlatformWindows();
		PLOGV << "ImGui::RenderPlatformWindowsDefault()";
		ImGui::RenderPlatformWindowsDefault();
	}
#endif

	PLOGV << "graphics.end_frame();";
	graphics.end_frame();
}
