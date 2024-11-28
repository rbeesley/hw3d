#include <3rdParty/ImGui/imgui.h>
#include <DirectXMath.h>
#include <random>

#include "App.hpp"

#include "AppThrowMacros.hpp"
#include "AtumMath.hpp"
#include "AtumException.hpp"
#include "Box.hpp"
#include "GDIPlusManager.hpp"
#include "Logging.hpp"
#include "Melon.hpp"
#include "Pyramid.hpp"
#include "Sheet.hpp"
#include "SkinnedBox.hpp"
#include "Surface.hpp"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

#if defined(LOG_WINDOW_MESSAGES) || defined(LOG_WINDOW_MOUSE_MESSAGES) // defined in LoggingConfig.hpp
#include "WindowsMessageMap.hpp"
const static class WindowsMessageMap windowsMessageMap;
#endif

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
	, stop_(false)
{
	PLOGI << "Constructing App";
	if (!window_->getHandle())
	{
		throw APP_EXCEPT("Failed to create Window");
	}

	gdiManager_ = GdiPlusManager::initialize();
}

int App::initialize()
{
	PLOGI << "Initializing App";
#if (IS_DEBUG)
	if (g_allowConsoleLogging)
	{
		console_ = std::make_unique<Console>(window_->getHandle(), TEXT("Debug Console"));

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
	}

	// Initialize the FPS Counter and CPU Counter
	fps_.initialize();
	cpu_.initialize();
#endif

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
#ifdef LOG_GRAPHICS_CALLS
				LOGV << "Drawable <Pyramid>     #" << ++count;
#endif
				return std::make_unique<Pyramid>(
					graphics_, rng_, distanceDistribution_, sphericalCoordinatePositionDistribution_, rotationOfDrawableDistribution_,
					sphericalCoordinateMovementOfDrawableDistribution_
				);
			case 1:
#ifdef LOG_GRAPHICS_CALLS
				LOGV << "Drawable <Box>         #" << ++count;
#endif
				return std::make_unique<Box>(
					graphics_, rng_, distanceDistribution_, sphericalCoordinatePositionDistribution_, rotationOfDrawableDistribution_,
					sphericalCoordinateMovementOfDrawableDistribution_, zAxisDistortionDistribution_
				);
			case 2:
#ifdef LOG_GRAPHICS_CALLS
				LOGV << "Drawable <Melon>       #" << ++count;
#endif
				return std::make_unique<Melon>(
					graphics_, rng_, distanceDistribution_, sphericalCoordinatePositionDistribution_, rotationOfDrawableDistribution_,
					sphericalCoordinateMovementOfDrawableDistribution_, latitudeDistribution_, longitudeDistribution_
				);
			case 3:
#ifdef LOG_GRAPHICS_CALLS
				LOGV << "Drawable <Sheet>       #" << ++count;
#endif
				return std::make_unique<Sheet>(
					graphics_, rng_, distanceDistribution_, sphericalCoordinatePositionDistribution_, rotationOfDrawableDistribution_, sphericalCoordinateMovementOfDrawableDistribution_
				);
			case 4:
#ifdef LOG_GRAPHICS_CALLS
				LOGV << "Drawable <SkinnedBox> #" << ++count;
#endif
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
	if (auto graphics = Window::getGraphicsWeakPtr().lock())
	{
		std::generate_n(std::back_inserter(drawables_), NUMBER_OF_DRAWABLES, DrawableFactory(*graphics, rng_seed));
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
					Window->getGraphics(), rng, distance_distribution, spherical_coordinate_position_distribution, rotation_of_drawable_distribution,
					spherical_coordinate_movement_of_drawable_distribution
				));
		}
#endif


		PLOGD << "Set graphics projection";
		graphics->setProjection(
			DirectX::XMMatrixPerspectiveLH(
				1.0f,
				aspectRatio,
				0.5f,
				40.0f));
	}
	return 0;
}

std::optional<unsigned int> App::processMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
#ifdef LOG_WINDOW_MESSAGES
		PLOGV << windowsMessageMap(msg.message, msg.lParam, msg.wParam).c_str();
#endif
		if (msg.message == WM_QUIT)
		{
			return msg.message;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

int App::run()
{
	bool showDemoWindow = true;
	bool showAnotherWindow = false;
	auto clearColor = ImVec4(0.07f, 0.0f, 0.12f, 1.00f);
	const ImGuiIO& imGuiIO = ImGui::GetIO();

	PLOGI << "Starting Message Pump and Render Loop";
	while (!stop_)
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
			if (const std::optional exitCode(processMessages()); exitCode.has_value())
			{
				if (exitCode.value() == WM_QUIT)
				{
					stop_ = true;
					continue;
				}
			}
		}

#ifdef LOG_GRAPHICS_CALLS
		PLOGV << "graphics.beginFrame()";
#endif
		{
			const auto [width, height] = window_->getTargetDimensions();
			const auto graphics = window_->getGraphicsWeakPtr().lock();
			if (!graphics->beginFrame(width, height))
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
#ifdef LOG_GRAPHICS_CALLS
		PLOGV << "ImGui_ImplDX11_NewFrame()";
#endif
		ImGui_ImplDX11_NewFrame();
#ifdef LOG_GRAPHICS_CALLS
		PLOGV << "ImGui_ImplWin32_NewFrame()";
#endif
		ImGui_ImplWin32_NewFrame();
#ifdef LOG_GRAPHICS_CALLS
		PLOGV << "ImGui::NewFrame()";
#endif
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (showDemoWindow)
		{
			PLOGD << "Show the big demo window";
			ImGui::ShowDemoWindow(&showDemoWindow);
		}

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

#ifdef LOG_GRAPHICS_CALLS
		PLOGV << "renderFrame(clear_color);";
#endif
		renderFrame(clearColor);

		if (const auto graphics = window_->getGraphicsWeakPtr().lock()) {
#ifdef LOG_GRAPHICS_CALLS
			PLOGV << "graphics->endFrame();";
#endif
			graphics->endFrame();
		}
	}
	return 0;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT App::handleMsg(const HWND hwnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) noexcept
{
#ifdef LOG_WINDOW_MESSAGES
	PLOGV << windowsMessageMap(msg, lParam, wParam).c_str();
#endif
#ifdef LOG_WINDOW_MOUSE_MESSAGES
	if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST)
	{
		PLOGV << windowsMessageMap(msg, lParam, wParam).c_str();
	}
#endif
#ifdef LOG_GRAPHICS_MESSAGES
	PLOGV << "ImGui_ImplWin32_WndProcHandler";
#endif
	{
		const auto graphics = Window::getGraphicsWeakPtr().lock();
		if (graphics && ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		{
			return true;
		}
	}

	switch (msg)
	{
		/* Window */
	case WM_CREATE:
	case WM_COMMAND:
	case WM_CLOSE:
	case WM_KILLFOCUS:
	case WM_SHOWWINDOW:
	case WM_MOVE:
	case WM_SIZE:
	case WM_ENTERSIZEMOVE:
	case WM_EXITSIZEMOVE:
	case WM_GETMINMAXINFO:
	case WM_SYSCOMMAND:
	case WM_DESTROY:
#ifdef IMGUI_DOCKING
	case WM_DPICHANGED:
#endif
		return Window::handleMsg(hwnd, msg, wParam, lParam);

		/* Keyboard */
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_CHAR:
		// Dear ImGui, a member of Graphics, gets the first opportunity to handle keyboard messages
		if (const auto graphics = Window::getGraphicsWeakPtr().lock()) {
			if (graphics->handleMsg(hwnd, msg, wParam, lParam))
			{
				return 0;
			}
		}
		// If Dear ImGui didn't process the keyboard message, Keyboard processes it.
		if (const auto keyboard = Window::getKeyboardWeakPtr().lock()) {
			keyboard->handleMsg(hwnd, msg, wParam, lParam);
		}
		break;

		/* Mouse */
		// WM_MOUSEMOVE is dependent on both the Window and the Mouse, so it is handled entirely in the App class
	case WM_MOUSEMOVE:
		// Dear ImGui, a member of Graphics, gets the first opportunity to handle Mouse messages
		if (const auto graphics = Window::getGraphicsWeakPtr().lock()) {
			if (graphics->handleMsg(hwnd, msg, wParam, lParam))
			{
				return 0;
			}
		}
		// If Dear ImGui didn't process the keyboard message, Keyboard processes it.
		if (const auto window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)))  // NOLINT(performance-no-int-to-ptr)
		if (auto mouse = Window::getMouseWeakPtr().lock())
		{
			const auto [x, y] = MAKEPOINTS(lParam);
			const auto [width, height] = window->getDimensions();
			// mouse is inside client region
			if (x >= 0 && x < width && y >= 0 && y < height)
			{
				// but internal state is still outside client region
				if (!mouse->isInWindow())
				{
					// fix the state
					mouse->onMouseEnter(x, y);
					SetCapture(hwnd);
				}
				else
				{
					mouse->onMouseMove(x, y);
				}
			}
			// mouse is outside client region and internal state places it inside client region
			else if (mouse->isInWindow())
			{
				// because a button is being held down we want to keep track of the mouse position outside the client region boundaries
				if (mouse->isLeftPressed() || mouse->isRightPressed() || mouse->isMiddlePressed() || mouse->isX1Pressed() || mouse->isX2Pressed())
				{
					mouse->onMouseMove(x, y);
				}
				// but no buttons ARE being held down
				else
				{
					// fix the state
					ReleaseCapture();
					mouse->onMouseLeave();
				}
			}
		}
		break;
		// All other mouse events can be handled by the Mouse itself
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		// Dear ImGui, a member of Graphics, gets the first opportunity to handle Mouse messages
		if (const auto graphics = Window::getGraphicsWeakPtr().lock()) {
			if (graphics->handleMsg(hwnd, msg, wParam, lParam))
			{
				return 0;
			}
		}
		// If Dear ImGui didn't process the keyboard message, Keyboard processes it.
		if (auto mouse = Window::getMouseWeakPtr().lock())
		{
			mouse->handleMsg(hwnd, msg, wParam, lParam);
		}
		break;

		/* App custom */
	case WM_APP:
		PLOGW << "Received WM_APP message";
		break;
	case WM_APP_CLOSE:
		PLOGW << "Received WM_APP_CLOSE message";
		break;
	case WM_APP_CONSOLE_CLOSE:
		PLOGW << "Handling WM_APP_CONSOLE_CLOSE message";
		::PostQuitMessage(0);
		break;
	case WM_APP_WINDOW_CLOSE:
		PLOGW << "Received WM_APP_WINDOW_CLOSE message";
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

App::~App()
{
	gdiManager_->shutdown();
	window_.reset();
#if (IS_DEBUG)
	console_->shutdown();
#endif
	PLOGI << "App destroyed";
}

void App::renderFrame(const ImVec4& clearColor)
{
	const auto dt = timer_.mark();

	if (const auto graphics = window_->getGraphicsWeakPtr().lock())
	{
		PLOGD << "Clear the buffer";
		graphics->clearBuffer(clearColor);

		PLOGD << "Draw all drawables";
		for (const auto& drawable : drawables_)
		{
			const auto keyboard = window_->getKeyboardWeakPtr().lock();
			drawable->update(keyboard->isKeyPressed(VK_SPACE) ? 0.0f : dt);
			drawable->draw(*graphics);
		}

#ifdef LOG_GRAPHICS_CALLS
		PLOGV << "ImGui::Render();";
#endif
		ImGui::Render();

		PLOGV << "ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData())";
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

#ifdef IMGUI_DOCKING
		PLOGD << "Fetch Dear ImGui IO for Docking";

		// Update and Render additional Platform Windows
		if (const ImGuiIO& imGuiIO = ImGui::GetIO(); imGuiIO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			PLOGV << "ImGui::UpdatePlatformWindows()";
			ImGui::UpdatePlatformWindows();
			PLOGV << "ImGui::RenderPlatformWindowsDefault()";
			ImGui::RenderPlatformWindowsDefault();
		}
#endif
	}
}
