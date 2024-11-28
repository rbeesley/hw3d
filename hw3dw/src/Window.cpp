#include "Window.hpp"

#include <system_error>

#include "App.hpp"
#include "Console.hpp"
#include "imgui.h"
#include "Logging.hpp"
#include "Resources/resource.h"
#include "WindowsThrowMacros.hpp"
#include "backends/imgui_impl_win32.h"

#if defined(LOG_WINDOW_MESSAGES) || defined(LOG_WINDOW_MOUSE_MESSAGES) // defined in LoggingConfig.h
#include "WindowsMessageMap.hpp"

const static class WindowsMessageMap windowsMessageMap;
#endif

// Definition of static member variables
SHORT Window::targetWidth_ = 0;
SHORT Window::targetHheight_ = 0;
bool Window::inSizeMove_ = false;
bool Window::minimized_ = false;

Window::WindowClass::WindowClass()
	: instanceHandle_(GetModuleHandle(nullptr))
{
	PLOGV << "Instantiate Window Class";

	WNDCLASSEX wcex = {
		.style = CS_OWNDC,
		.lpfnWndProc = handleMsgSetup,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = getInstance(),
		.hIcon = static_cast<HICON>(LoadImage(instanceHandle_, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), 0)),
		.hCursor = nullptr,
		.hbrBackground = nullptr,
		.lpszMenuName = nullptr,
		.lpszClassName = getName(),
		.hIconSm = static_cast<HICON>(LoadImage(instanceHandle_, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0))
	};
	wcex.cbSize = sizeof(wcex);

	RegisterClassEx(&wcex);
}

void Window::WindowClass::shutdown() const noexcept
{
	PLOGD << "Shutdown Window Class";
	UnregisterClass(windowClassName, getInstance());
}

Window::WindowClass::~WindowClass()
{
	PLOGV << "Destroy Window Class";
}

LPCWSTR Window::WindowClass::getName() noexcept
{
	return windowClassName;
}

HINSTANCE Window::WindowClass::getInstance() const noexcept
{
	return instanceHandle_;
}

// Define the static members
std::shared_ptr<Mouse> Window::mouse_ = nullptr;
std::shared_ptr<Keyboard> Window::keyboard_ = nullptr;
std::shared_ptr<Graphics> Window::graphics_ = nullptr;

Window::Window(const int width, const int height, const LPCWSTR name)
	: width_(width)
	, height_(height)
	, windowClass_(std::make_unique<WindowClass>()) {
	PLOGI << "Initialize Window";

	initializeStaticMembers();

	createWindow(name);

	configureImGui();

	// Create the graphics object
	PLOGD << "Create the DirectX graphics object";
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "graphics_ = std::make_shared<Graphics>(windowHandle_, width_, height_);";
#endif
	graphics_ = std::make_shared<Graphics>(windowHandle_, width_, height_);

	// Check for an error
	if (nullptr == graphics_)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	if(targetWidth_ == width_ && targetHheight_ == height_)
	{
		setTargetDimensions(0, 0);
	}
}

Window::~Window()
{
	PLOGD << "Destroy Window";
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "graphics_->shutdown();";
#endif
	graphics_->shutdown();
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "ImGui_ImplWin32_Shutdown();";
#endif
	ImGui_ImplWin32_Shutdown();
	windowClass_->shutdown();	PLOGV << "ImGui::DestroyContext()";
	ImGui::DestroyContext();
	DestroyWindow(windowHandle_);
	shutdownStaticMembers();
}

void Window::createWindow(const LPCWSTR name)
{
	// ReSharper disable once CppInitializedValueIsAlwaysRewritten
	RECT windowRect{};
	windowRect.left = 100;
	windowRect.top = 100;
	windowRect.right = width_ + windowRect.left;
	windowRect.bottom = height_ + windowRect.top;

	if (!AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	windowHandle_ = CreateWindowEx(
		0,
		WindowClass::getName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		windowClass_->getInstance(),
		this
	);

	// Check for an error
	if (nullptr == windowHandle_)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	// Newly created windows start off as hidden
	PLOGI << "Show the Window";
	ShowWindow(windowHandle_, SW_SHOWDEFAULT);
	UpdateWindow(windowHandle_);
}

void Window::configureImGui()
{
	// Configure Dear ImGui
	PLOGI << "Configure Dear ImGui";

	// Setup Dear ImGui context
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "IMGUI_CHECKVERSION();";
#endif
	IMGUI_CHECKVERSION();
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "ImGui::CreateContext();";
#endif
	ImGui::CreateContext();
	PLOGD << "Set Dear ImGui flags";
	ImGuiIO& imGuiIo = ImGui::GetIO();
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
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "ImGui_ImplWin32_Init()";
#endif
	ImGui_ImplWin32_Init(windowHandle_);
}

void Window::initializeStaticMembers()
{
	mouse_ = std::make_shared<Mouse>();
	keyboard_ = std::make_shared<Keyboard>();
}

void Window::shutdownStaticMembers()
{
	mouse_.reset();
	keyboard_.reset();
}

HWND Window::getHandle()
{
	return windowHandle_;
}

HWND Window::setActive(const HWND window)
{
	if (mouse_->isInWindow())
	{
		if (const HWND active_window = GetActiveWindow(); active_window != window)
		{
			return SetActiveWindow(window);
		}
	}

	return nullptr;
}

Window::WindowDimensions Window::getDimensions() const
{
	return { width_, height_ };
}

Window::WindowPosition Window::getPosition()
{
	return { x_, y_ };
}

void Window::setPosition(const int x, const int y)
{
	x_ = x;
	y_ = y;
}

Window::WindowDimensions Window::getTargetDimensions()
{
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "getTargetDimensions() : width: " << targetWidth_ << ", height: " << targetHheight_ << "";
#endif
	return { targetWidth_, targetHheight_ };
}

void Window::setTargetDimensions(const unsigned int width, const unsigned int height)
{
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "setTargetDimensions(width: " << width << ", height: " << height << ");";
#endif
	targetWidth_ = width;
	targetHheight_ = height;
}

void Window::setTitle(const std::wstring& title)
{
	if (!SetWindowText(windowHandle_, title.c_str()))
	{
		throw ATUM_WND_LAST_EXCEPT();
	}
}

std::weak_ptr<Mouse> Window::getMouseWeakPtr() noexcept
{
	return mouse_;
}

std::weak_ptr<Keyboard> Window::getKeyboardWeakPtr() noexcept
{
	return keyboard_;
}

std::weak_ptr<Graphics> Window::getGraphicsWeakPtr() noexcept
{
	return graphics_;
}

LRESULT Window::handleMsg(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
#ifdef LOG_WINDOW_MESSAGES
	PLOGV << windowsMessageMap(msg, lParam, wParam).c_str();
#endif
	switch (msg)
	{
	case WM_CREATE:
		RECT rect;
		GetWindowRect(window, &rect);
		setPosition(rect.left, rect.top);
		break;
	case WM_COMMAND:
	{
		const int windowMessageId = LOWORD(wParam);
		// ReSharper disable once CppDeclaratorNeverUsed
		const int windowMessageEvent = HIWORD(wParam);
		switch (windowMessageId)
		{
		case 0:
		default:
			return DefWindowProc(window, msg, wParam, lParam);
		}
	}
	case WM_CLOSE:
		PLOGW << "Handling WM_CLOSE message";
		::PostQuitMessage(0);
		return 0;
	case WM_KILLFOCUS:
		keyboard_->clearState();
		break;
	case WM_SHOWWINDOW:
		if (wParam == TRUE) // The window is being shown
		{
			RECT rect;
			GetWindowRect(window, &rect);
			setPosition(rect.left, rect.top);
		}
		break;
	case WM_MOVE:
	{
		const int x_pos = static_cast<short>(LOWORD(lParam)); // Horizontal position
		const int y_pos = static_cast<short>(HIWORD(lParam)); // Vertical position

		// Calculate how far the window has moved
		if (const auto mouse = getMouseWeakPtr().lock())
		{
			auto [x, y] = getPosition();
			const int x_delta = x_pos - x;
			const int y_delta = y_pos - y;

#ifdef LOG_WINDOW_MOVEMENT_MESSAGES
			PLOGD << "Window moved: " << x_delta << ", " << y_delta;
#endif

			// Update the mouse position so that the elements in the window which are dependent on the mouse position, are updated to appear stationary
			mouse->onMouseMove(mouse->pos().x - x_delta, mouse->pos().y - y_delta);
		}

		// Store the new position for future calculations
		setPosition(x_pos, y_pos);
		break;
	}
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			// The window was minimized (you should probably suspend the application)
			if (!minimized_)
			{
				minimized_ = true;
			}
		}
		else if (minimized_)
		{
			// The window was minimized and is now restored (resume from suspend)
			minimized_ = false;
		}
		if (!inSizeMove_)
		{
			// Handle the swapchain resize for maximize or unmaximize
			setTargetDimensions(LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_ENTERSIZEMOVE:
		// We want to avoid trying to resizing the swapchain as the user does the 'rubber band' resize
		inSizeMove_ = true;
		break;
	case WM_EXITSIZEMOVE:
		// Here is the other place where you handle the swapchain resize after the user stops using the 'rubber-band'
		inSizeMove_ = false;
		break;
	case WM_GETMINMAXINFO:
	{
		// We want to prevent the window from being set too tiny
		const auto info = reinterpret_cast<MINMAXINFO*>(lParam);
		info->ptMinTrackSize.x = 320;
		info->ptMinTrackSize.y = 200;
		break;
	}
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PLOGW << "Handling WM_DESTROY message";
		break;
#ifdef IMGUI_DOCKING
	case WM_DPICHANGED:
		if (ImGuiIO& imGuiIo = ImGui::GetIO(); imGuiIo.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			const int dpi = HIWORD(wParam);
			PLOGI << "WM_DPICHANGED to " << dpi << "(" << static_cast<float>(dpi) / 96.0f * 100.0f << ")";
			const RECT* suggested_rect = reinterpret_cast<RECT*>(lParam);
			::SetWindowPos(windowHandle_, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
#endif
	default:
		break;
	}
	return DefWindowProc(window, msg, wParam, lParam);
}

LRESULT CALLBACK Window::handleMsgSetup(const HWND window, const UINT msg, const WPARAM wParam, const LPARAM lParam) noexcept
{
#ifdef LOG_WINDOW_MESSAGES // defined in LoggingConfig.h
	PLOGV << windowsMessageMap(msg, lParam, wParam).c_str();
#endif

	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const p_create = reinterpret_cast<CREATESTRUCTW*>(lParam);  // NOLINT(performance-no-int-to-ptr)
		Window* const p_wnd = static_cast<Window*>(p_create->lpCreateParams);
		SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(p_wnd));
		SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::handleMsgThunk));
		return handleMsg(window, msg, wParam, lParam);
	}
	return DefWindowProc(window, msg, wParam, lParam);
}

LRESULT CALLBACK Window::handleMsgThunk(const HWND window, const UINT msg, const WPARAM wParam, const LPARAM lParam) noexcept
{
#ifdef LOG_WINDOW_MESSAGES // defined in LoggingConfig.h
	PLOGV << windowsMessageMap(msg, lParam, wParam).c_str();
#endif
	if (msg == WM_CREATE)
	{
		auto result = handleMsg(window, msg, wParam, lParam);
		SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&App::handleMsg));
		return result;
	}
	return handleMsg(window, msg, wParam, lParam);
}

Window::HResultException::HResultException(const int line, const char* file, const HRESULT hresult) noexcept
	:
	Exception(line, file),
	hresult_(hresult)
{}

const char* Window::HResultException::what() const noexcept
{
	std::ostringstream out;
	out << "[Error Code] " << getErrorCode() << "\n"
		<< "[Description] " << getErrorDescription() << "\n"
		<< getOriginString();
	whatBuffer_ = out.str();
	return whatBuffer_.c_str();
}

const char* Window::HResultException::getType() const noexcept
{
	return "Atum Window Exception";
}

std::string Window::Exception::translateErrorCode(const HRESULT result) noexcept
{
	std::string message = std::system_category().message(result);
	return message;
}

HRESULT Window::HResultException::getErrorCode() const noexcept
{
	return hresult_;
}

std::string Window::HResultException::getErrorDescription() const noexcept
{
	return translateErrorCode(hresult_);
}

const char* Window::NoGfxException::getType() const noexcept
{
	return "Atum Window Exception [No Graphics]";
}
