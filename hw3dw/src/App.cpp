#include <3rdParty/ImGui/imgui.h>
#include <DirectXMath.h>
#include <random>

#include "App.h"

#include "AppThrowMacros.h"
#include "AtumMath.h"
#include "AtumException.h"
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

extern bool g_allowConsoleLogging;

constexpr size_t NUMBER_OF_DRAWABLES = 180;

// 1280x720
// 800x600
constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;

constexpr float aspectRatio = static_cast<float>(HEIGHT) / static_cast<float>(WIDTH);

App::AppException::AppException(const int line, const char* file, const std::string& msg) noexcept
	: Exception(line, file)
{
	whatBuffer_.assign(msg);
}

const char* App::AppException::getType() const noexcept
{
	return "Atum Application Exception";
}

App::App()
	: window_(std::make_unique<Window>(WIDTH, HEIGHT, TEXT("Atum D3D Window")))
{
	PLOGI << "Initializing App";
	mouse_ = window_->get_mouse();
	keyboard_ = window_->get_keyboard();
	graphics_ = window_->get_graphics();

#if (IS_DEBUG)
	console_ = std::make_unique<Console>(TEXT("Debug Console"));
	if (g_allowConsoleLogging)
	{
		auto exe_path = []
		{
			TCHAR buffer[MAX_PATH] = {0};
			GetModuleFileName(nullptr, buffer, MAX_PATH);
			const std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
			return std::wstring(buffer).substr(0, pos);
		};

		// Check Logging
		// This is the earliest this can be done if utilizing the console logger.
		PLOGN << "CWD: " << exe_path();
		PLOGN << "";
		PLOGN << "This is a NONE message";
		PLOGF << "This is a FATAL message";
		PLOGE << "This is an ERROR message";
		PLOGW << "This is a WARNING message";
		PLOGI << "This is an INFO message";
		PLOGD << "This is a DEBUG message";
		PLOGV << "This is a VERBOSE message";
		PLOGN << "";

		// If the console failed to load, we want to make sure this is the last log message so that it is easier to spot.
		if (!console_->getHandle())
		{
			throw APP_EXCEPT("Failed to create Debug Console");
		}

		// Initialize the FPS Counter and CPU Counter
		fps_.initialize();
		cpu_.initialize();
	}
#endif

	if (!window_->getHandle())
	{
		throw APP_EXCEPT("Failed to create Window");
	}

	gdiManager_ = GdiPlusManager::initialize();
}

int App::initialize()
{
	const auto rng_seed = std::random_device{}();
	PLOGI << "mt19937 rng seed: " << rng_seed;

#if true // if set to false, don't use the factory and draw a single drawable
	class DrawableFactory
	{
	public:
		DrawableFactory(Graphics& graphics, const unsigned int rng_seed)
			:
			graphics_(graphics),
			rng_(rng_seed)
		{}

		int count = 0;
		std::unique_ptr<Drawable> operator()()
		{
			switch (drawableTypeDistribution_(rng_))
			{
			case 0:
				LOGV << "Drawable <Pyramid>     #" << ++count;
				return std::make_unique<Pyramid>(
					graphics_, rng_, distanceDistribution_, sphericalCoordinatePositionDistribution_, rotationOfDrawableDistribution_,
					sphericalCoordinateMovementOfDrawableDistribution_
				);
			case 1:
				LOGV << "Drawable <Box>         #" << ++count;
				return std::make_unique<Box>(
					graphics_, rng_, distanceDistribution_, sphericalCoordinatePositionDistribution_, rotationOfDrawableDistribution_,
					sphericalCoordinateMovementOfDrawableDistribution_, zAxisDistortionDistribution_
				);
			case 2:
				LOGV << "Drawable <Melon>       #" << ++count;
				return std::make_unique<Melon>(
					graphics_, rng_, distanceDistribution_, sphericalCoordinatePositionDistribution_, rotationOfDrawableDistribution_,
					sphericalCoordinateMovementOfDrawableDistribution_, latitudeDistribution_, longitudeDistribution_
				);
			case 3:
				LOGV << "Drawable <Sheet>       #" << ++count;
				return std::make_unique<Sheet>(
					graphics_, rng_, distanceDistribution_, sphericalCoordinatePositionDistribution_, rotationOfDrawableDistribution_, sphericalCoordinateMovementOfDrawableDistribution_
				);
			case 4:
				LOGV << "Drawable <SkinnedBox> #" << ++count;
				return std::make_unique<SkinnedBox>(
					graphics_, rng_, distanceDistribution_, sphericalCoordinatePositionDistribution_, rotationOfDrawableDistribution_,
					sphericalCoordinateMovementOfDrawableDistribution_
				);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}	private:
			Graphics& graphics_;
			std::mt19937 rng_;
			std::uniform_real_distribution<float> sphericalCoordinatePositionDistribution_{ 0.0f, PI * 2.0f };				// adist
			std::uniform_real_distribution<float> rotationOfDrawableDistribution_{ 0.0f, PI * 0.5f };						// ddist
			std::uniform_real_distribution<float> sphericalCoordinateMovementOfDrawableDistribution_{ 0.0f, PI * 0.08f };	// odist
			std::uniform_real_distribution<float> distanceDistribution_{ 6.0f, 20.0f };									// rdist
			std::uniform_real_distribution<float> zAxisDistortionDistribution_{ 0.4f, 3.0f };								// bdist
			std::uniform_int_distribution<int> latitudeDistribution_{ 5, 20 };												// latdist
			std::uniform_int_distribution<int> longitudeDistribution_{ 10, 40 };											// longdist
			std::uniform_int_distribution<int> drawableTypeDistribution_{ 0, 4 };											// typedist
	};

	drawables_.reserve(NUMBER_OF_DRAWABLES);

	PLOGD << "Populating pool of drawables";
	std::generate_n(std::back_inserter(drawables_), NUMBER_OF_DRAWABLES, DrawableFactory(*graphics_, rng_seed));
#else
	{
		PLOGD << "Draw a SkinnedBox";
		std::mt19937 rng{ rng_seed };
		std::uniform_real_distribution<float> distance_distribution{ 0.0f, 0.0f };
		//std::uniform_real_distribution<float> spherical_coordinate_position_distribution{ 0.0f, PI * 2.0f };
		std::uniform_real_distribution<float> spherical_coordinate_position_distribution{ 0.0f, 0.0f };
		std::uniform_real_distribution<float> rotation_of_drawable_distribution{ 0.0f, PI * 0.5f };
		//std::uniform_real_distribution<float> rotation_of_drawable_distribution{ 0.0f, 0.0f };
		//std::uniform_real_distribution<float> spherical_coordinate_movement_of_drawable_distribution{ 0.0f, PI * 0.08f };
		std::uniform_real_distribution<float> spherical_coordinate_movement_of_drawable_distribution{ 0.0f, 0.0f };
		drawables_.emplace_back(
			std::make_unique<SkinnedBox>(
				Window->get_graphics(), rng, distance_distribution, spherical_coordinate_position_distribution, rotation_of_drawable_distribution,
				spherical_coordinate_movement_of_drawable_distribution
			));
	}
#endif

	PLOGD << "Set graphics projection";
	graphics_->setProjection(
		DirectX::XMMatrixPerspectiveLH(
			1.0f,
			aspectRatio,
			0.5f,
			40.0f));

	return 0;
}

int App::run()
{
	bool showDemoWindow = true;
	bool showAnotherWindow = false;
	auto clearColor = ImVec4(0.07f, 0.0f, 0.12f, 1.00f);
	const ImGuiIO& imGuiIO = ImGui::GetIO();

	PLOGI << "Starting Message Pump and Render Loop";
	constexpr bool done = false;
	while (!done)
	{
		//PLOGI << " *** Run loop ***";

		#if (IS_DEBUG)
		auto static composeFpsCpuWindowTitle = [](const int fps, const double cpuPercentage) {
			wchar_t buffer[50];
			std::swprintf(buffer, 50, L"fps: %d / cpu: %00.2f%%", fps, cpuPercentage);
			return std::wstring(buffer);
			};

		fps_.frame();
		cpu_.frame();
		window_->setTitle(composeFpsCpuWindowTitle(fps_.getFps(), cpu_.getCpuPercentage()));
#endif

		// Process pending messages
		{
			if (const std::optional exitCode(window_->processMessages()); exitCode.has_value())
			{
				if (exitCode.value() == WM_QUIT)
				{
					return 0;
				}
			}
		}

		PLOGV << "Window->getGraphics().beginFrame()";
		{
			if (auto [width, height] = window_->getTargetDimensions(); !graphics_->beginFrame(width, height))
			{
				if (width > 0 || height > 0)
				{
					window_->setTargetDimensions(0, 0);
				}
				continue;
			}
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
		if (showDemoWindow)
			PLOGD << "Show the big demo window";
			ImGui::ShowDemoWindow(&showDemoWindow);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			PLOGD << "Show a simple Dear ImGui window";
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &showAnotherWindow);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&clearColor)); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / imGuiIO.Framerate, imGuiIO.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (showAnotherWindow)
		{
			PLOGD << "Show another simple Dear ImGui window";
			ImGui::Begin("Another Window", &showAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				showAnotherWindow = false;
			ImGui::End();
		}

		PLOGV << "ImGui::Render();";
		ImGui::Render();
		PLOGV << "renderFrame(clear_color);";
		renderFrame(clearColor);

		PLOGV << "Graphics.endFrame();";
		graphics_->endFrame();
	}
}

App::~App()
{
	gdiManager_->shutdown();
#if (IS_DEBUG)
	console_->shutdown();
#endif
}

void App::renderFrame(const ImVec4& clearColor)
{
	const auto dt = timer_.mark();

	PLOGD << "Fetch Dear ImGui IO";
	const ImGuiIO& imGuiIO = ImGui::GetIO(); (void)imGuiIO;

	PLOGD << "Clear the buffer";
	graphics_->clearBuffer(clearColor);

	PLOGD << "Draw all drawables";
	for (const auto& drawable : drawables_)
	{
		drawable->update(keyboard_->isKeyPressed(VK_SPACE) ? 0.0f : dt);
		drawable->draw(*graphics_);
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
}
