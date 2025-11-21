#include <3rdParty/ImGui/imgui.h>
#include <DirectXMath.h>
#include <random>

#include "App.hpp"
#include "AppConfig.hpp"
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

#define IMGUI_DOCKING

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

constexpr float aspectRatio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

// --- Exception Definitions ---
App::AppException::AppException(const int line, const char* file, const std::string& msg) noexcept
	: Exception(line, file)
{
	whatBuffer_.assign(msg);
}

const char* App::AppException::getType() const noexcept
{
	return "Atum Application Exception";
}

// --- Lifecycle ---
App::App(bool allowConsoleLogging)
	: window_(std::make_unique<Window>(WIDTH, HEIGHT, TEXT("Atum D3D Window")))
	, stop_(false)
{
	PLOGI << "Constructing App";

	if (!window_->getHandle())
	{
		throw APP_EXCEPT("Failed to create Window");
	}

	graphics_ = &window_->getGraphics();
	mouse_ = &window_->getMouse();
	keyboard_ = &window_->getKeyboard();

#if (IS_DEBUG)
	if (allowConsoleLogging)
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
		PLOGN << "PWD: " << exe_path();
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

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	configureImGui();

	PLOGD << "Set the camera view";
	graphics_->setCamera(std::make_unique<Camera>(DirectX::XM_PIDIV4, aspectRatio, 0.5f, 40.0f));
	graphics_->getCamera()->setPosition({ 0.0f, 0.0f, 0.0f });
	graphics_->getCamera()->setTarget({ 0.0f, 0.0f, 5.0f });
	graphics_->getCamera()->setFOV(DirectX::XM_PIDIV4); // Zoom

	gdiManager_ = std::make_unique<GdiPlusManager>();
	populateDrawables();
}

App::~App()
{
#ifdef IMGUI_DOCKING
	// Disable platform windows before shutdown
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;

	// BUGBUG: It is recommended that this frame is rendered after changing the viewports flag,
	// but that introduces other complications
	//window_->updateImGuiPlatform();
	//graphics_->renderImGuiPlatform();
#endif

	Graphics::ImGui::Shutdown();
	Window::ImGui::Shutdown();

#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "ImGui::DestroyContext()";
#endif
	ImGui::DestroyContext();

	window_.reset();
#if (IS_DEBUG)
	console_->shutdown();
#endif
	PLOGI << "App destroyed";
}

// --- Main Loop ---
int App::run()
{
	bool showDemoWindow = true;
	bool showAnotherWindow = false;
	auto clearColor = ImVec4(0.07f, 0.0f, 0.12f, 1.00f);
	const ImGuiIO& io = ImGui::GetIO();

#if defined(MAX_FPS) && (MAX_FPS > 0)
	using clock = std::chrono::steady_clock; // Monotonic clock, unaffected by system time changes
	const std::chrono::seconds interval(MAX_FPS);  // 1 FPS = 1 second per frame
	auto next_tick = clock::now();
#endif

	PLOGI << "Starting Message Pump and Render Loop";
	while (!stop_)
	{
		// PLOGI << " *** Run loop ***";

		// Process pending messages
		if (const std::optional exitCode(processMessages()); exitCode.has_value())
		{
			if (exitCode.value() == WM_QUIT)
			{
				stop_ = true;
				continue;
			}
		}

#if defined(MAX_FPS) && (MAX_FPS > 0)
		auto now = clock::now();
		std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char timeStr[26]; // ctime_s requires a buffer of at least 26 bytes
		errno_t err = ctime_s(timeStr, sizeof(timeStr), &t);
		if (err == 0) {
			PLOGV << "Tick at: " << timeStr; // already includes newline
		}
		else {
			PLOGW << "Failed to format time string";
		}

		// Schedule next tick
		next_tick += interval;
		// Sleep until the next scheduled time
		std::this_thread::sleep_until(next_tick);
#endif

#if (IS_DEBUG)
		fps_.frame();
		cpu_.frame();
		window_->setTitle(L"fps: " + std::to_wstring(fps_.getFps()) + L" / cpu: " + std::to_wstring(cpu_.getCpuPercentage()) + L"%");
#endif

#ifdef LOG_GRAPHICS_CALLS
		PLOGV << "graphics.beginFrame()";
#endif
		const auto [width, height] = window_->getTargetDimensions();
		if (!graphics_->beginFrame(width, height))
		{
			if (width > 0 || height > 0)
			{
				window_->setTargetDimensions(0, 0);
			}
			continue;
		}

		// Start the Dear ImGui frame
		PLOGD << "Start the Dear ImGui frame";
		Graphics::ImGui::NewFrame();
		Window::ImGui::NewFrame();
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

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
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
		PLOGV << "ImGui::Render();";
#endif
		ImGui::Render();

#ifdef LOG_GRAPHICS_CALLS
		PLOGV << "renderFrame(clear_color);";
#endif
		renderFrame(clearColor);

#ifdef LOG_GRAPHICS_CALLS
		PLOGV << "graphics->endFrame();";
#endif
		graphics_->endFrame();
	}
	return 0;
}

// --- ImGui Configuration ---
void App::configureImGui() const
{
	// Configure Dear ImGui
	PLOGI << "Configure Dear ImGui";

	PLOGD << "Set Dear ImGui flags";
	ImGuiIO& imGuiIo = ImGui::GetIO(); (void)imGuiIo;
	imGuiIo.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	imGuiIo.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#ifdef IMGUI_DOCKING
	imGuiIo.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	imGuiIo.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//imGuiIo.ConfigViewportsNoAutoMerge = true;
	//imGuiIo.ConfigViewportsNoTaskBarIcon = true;
	//imGuiIo.ConfigViewportsNoDefaultParent = true;
	//imGuiIo.ConfigDockingAlwaysTabBar = true;
	//imGuiIo.ConfigDockingTransparentPayload = true;
	//imGuiIo.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	//imGuiIo.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.
#endif

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

#ifdef IMGUI_DOCKING
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (imGuiIo.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
#endif

	PLOGI << "Setup Dear ImGui Platform backend";
	Window::ImGui::Init(window_->getHandle());
	Graphics::ImGui::Init(graphics_->getDevice(), graphics_->getDeviceContext());
}

// --- Message Handling ---
LRESULT App::handleMsg(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		App* app = static_cast<App*>(cs->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
	}

	App* app = reinterpret_cast<App*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (app) {
		return app->handleMsgImpl(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT App::handleMsgImpl(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) noexcept
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
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
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
	case WM_DPICHANGED:
		return window_->forwardMsg(hWnd, msg, wParam, lParam);

		/* Keyboard */
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_CHAR:
		// Dear ImGui, a member of Graphics, gets the first opportunity to handle keyboard messages
		if (graphics_->handleMsg(hWnd, msg, wParam, lParam))
		{
			return 0;
		}
		// If Dear ImGui didn't process the keyboard message, Keyboard processes it.
		keyboard_->handleMsg(hWnd, msg, wParam, lParam);
		break;

		/* Mouse */
		// WM_MOUSEMOVE is dependent on both the Window and the Mouse, so it is handled entirely in the App class
	case WM_MOUSEMOVE:
		// Dear ImGui, a member of Graphics, gets the first opportunity to handle Mouse messages
		if (graphics_->handleMsg(hWnd, msg, wParam, lParam))
		{
			return 0;
		}
		// If Dear ImGui didn't process the mouse message, Mouse processes it.
		if (const auto window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)))  // NOLINT(performance-no-int-to-ptr)
		{
			const auto [x, y] = MAKEPOINTS(lParam);
			const auto [width, height] = window->getDimensions();
			// mouse is inside client region
			if (x >= 0 && x < width && y >= 0 && y < height)
			{
				// but internal state is still outside client region
				if (!mouse_->isInWindow())
				{
					// fix the state
					mouse_->onMouseEnter(x, y);
					SetCapture(hWnd);
				}
				else
				{
					mouse_->onMouseMove(x, y);
				}
			}
			// mouse is outside client region and internal state places it inside client region
			else if (mouse_->isInWindow())
			{
				// because a button is being held down we want to keep track of the mouse position outside the client region boundaries
				if (mouse_->isLeftPressed() || mouse_->isRightPressed() || mouse_->isMiddlePressed() || mouse_->isX1Pressed() || mouse_->isX2Pressed())
				{
					mouse_->onMouseMove(x, y);
				}
				// but no buttons ARE being held down
				else
				{
					// fix the state
					ReleaseCapture();
					mouse_->onMouseLeave();
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
		if (graphics_->handleMsg(hWnd, msg, wParam, lParam))
		{
			return 0;
		}
		// If Dear ImGui didn't process the mouse message, Mouse processes it.
		mouse_->handleMsg(hWnd, msg, wParam, lParam);
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
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// --- Helpers ---
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

void App::renderFrame(const ImVec4& clearColor)
{
	const auto dt = timer_.mark();
	PLOGD << "Fetch Dear ImGui IO";
	const ImGuiIO& io = ImGui::GetIO(); (void)io;

	PLOGD << "Clear the buffer";
	graphics_->clearBuffer(clearColor);

#define CAMERA_ZOOM false
#if (CAMERA_ZOOM)
	static float timeAccumulator = 0.0f;
	timeAccumulator += dt;

	// Zoom oscillates between - 10 and -10 over 5 seconds
	float zoomAmplitude = 16.0f; // range of movement
	float zoomBase = -8.0f;     // center position
	float zoomSpeed = DirectX::XM_2PI / 5.0f; // full cycle every 5 seconds

	float zoomZ = zoomBase + std::sin(timeAccumulator * zoomSpeed) * (zoomAmplitude / 2.0f);

	auto camera = graphics_->getCamera();
	DirectX::XMFLOAT3 pos = camera->getPosition();
	pos.z = zoomZ;
	camera->setPosition(pos);
#endif

	PLOGD << "Draw all drawables";
	for (const auto& drawable : drawables_)
	{
		//drawable->update(keyboard->isKeyPressed(VK_SPACE) ? 0.0f : dt);
		drawable->update(dt);
		drawable->draw(*graphics_);
	}

	PLOGV << "ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData())";
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

#ifdef IMGUI_DOCKING
	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		PLOGV << "ImGui::UpdatePlatformWindows()";
		ImGui::UpdatePlatformWindows();
		PLOGV << "ImGui::RenderPlatformWindowsDefault()";
		ImGui::RenderPlatformWindowsDefault();
	}
#endif
}

void App::populateDrawables()
{
	const auto rng_seed = std::random_device{}();
	PLOGI << "mt19937 rng seed: " << rng_seed;

#define PROTOTYPE_DRAWABLE false // if set to true, don't use the factory and draw a single drawable
#if (PROTOTYPE_DRAWABLE)
	PLOGD << "Draw a Prototype Drawable";
	std::mt19937 rng_{ rng_seed };
	std::uniform_real_distribution<float> distanceDistribution_{ 0.0f, 0.0f };
	//std::uniform_real_distribution<float> sphericalCoordinatePositionDistribution_{ 0.0f, PI * 2.0f };
	std::uniform_real_distribution<float> sphericalCoordinatePositionDistribution_{ 0.0f, 0.0f };
	std::uniform_real_distribution<float> rotationOfDrawableDistribution_{ 0.0f, PI * 0.5f };
	//std::uniform_real_distribution<float> rotationOfDrawableDistribution_{ 0.0f, 0.0f };
	//std::uniform_real_distribution<float> sphericalCoordinateMovementOfDrawableDistribution_{ 0.0f, PI * 0.08f };
	std::uniform_real_distribution<float> sphericalCoordinateMovementOfDrawableDistribution_{ 0.0f, 0.0f };
	drawables_.emplace_back(
		std::make_unique<SkinnedBox>(
			*graphics_, rng_, distanceDistribution_, sphericalCoordinatePositionDistribution_, rotationOfDrawableDistribution_,
			sphericalCoordinateMovementOfDrawableDistribution_
		));
#else
	class DrawableFactory
	{
	public:
		DrawableFactory(Graphics& graphics, const unsigned int rng_seed)
			:
			graphics_(graphics),
			rng_(rng_seed)
		{
		}

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
	std::generate_n(std::back_inserter(drawables_), NUMBER_OF_DRAWABLES, DrawableFactory(*graphics_, rng_seed));
#endif
}
