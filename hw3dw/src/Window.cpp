#include "Window.hpp"

#include <system_error>

#include "App.hpp"
#include "Console.hpp"
#include "Logging.hpp"
#include "Resources/resource.h"
#include "WindowsThrowMacros.hpp"

#if defined(LOG_WINDOW_MESSAGES) || defined(LOG_WINDOW_MOUSE_MESSAGES) // defined in LoggingConfig.h
#include "WindowsMessageMap.hpp"

const static class WindowsMessageMap windowsMessageMap;
#endif

// -----------------------------
// Window Class Implementation
// -----------------------------
// Lifecycle
Window::Window(const unsigned int width, const unsigned int height, const LPCWSTR name)
	: width_(width)
	, height_(height)
	, windowClass_(std::make_unique<WindowClass>()) {
	PLOGI << "Initialize Window";

	mouse_ = std::make_unique<Mouse>();
	keyboard_ = std::make_unique<Keyboard>();

	createWindow(name);

	graphics_ = std::make_unique<Graphics>(windowHandle_, width_, height_);

	// Check for an error
	if (!graphics_)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	if(targetWidth_ == width_ && targetHeight_ == height_)
	{
		setTargetDimensions(0, 0);
	}
}

Window::~Window()
{
	PLOGD << "Destroy Window";
	DestroyWindow(windowHandle_);
}

// Message Handling
LRESULT Window::forwardMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return handleMsgImpl(hWnd, msg, wParam, lParam);
}

// Accessors
HWND Window::getHandle() const noexcept { return windowHandle_; }
Graphics& Window::getGraphics() const noexcept { return *graphics_; }
Mouse& Window::getMouse() const noexcept { return *mouse_; }
Keyboard& Window::getKeyboard() const noexcept { return *keyboard_; }

// Window Management
void Window::setTitle(const std::wstring& title) const
{
	if (!SetWindowText(windowHandle_, title.c_str()))
	{
		throw ATUM_WND_LAST_EXCEPT();
	}
}

HWND Window::setActive(const HWND window) const
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

void Window::createWindow(const LPCWSTR name)
{
	// ReSharper disable once CppInitializedValueIsAlwaysRewritten
	RECT rect = { 100, 100, 100 + width_, 100 + height_ };

	if (!AdjustWindowRect(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	windowHandle_ = CreateWindowEx(
		0,
		windowClass_->getName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		windowClass_->getInstance(),
		this
	);

	// Check for an error
	if (!windowHandle_)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	// Newly created windows start off as hidden
	PLOGI << "Show the Window";
	ShowWindow(windowHandle_, SW_SHOWDEFAULT);
	UpdateWindow(windowHandle_);
}

// Dimensions and Position
Window::WindowDimensions Window::getDimensions() const
{
	return { width_, height_ };
}

Window::WindowPosition Window::getPosition() const
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
	PLOGV << "getTargetDimensions() : width: " << targetWidth_ << ", height: " << targetHeight_ << "";
#endif
	return { targetWidth_, targetHeight_ };
}

void Window::setTargetDimensions(const unsigned int width, const unsigned int height)
{
#ifdef LOG_GRAPHICS_CALLS
	PLOGV << "setTargetDimensions(width: " << width << ", height: " << height << ");";
#endif
	targetWidth_ = width;
	targetHeight_ = height;
}

// Exception Types
Window::HResultException::HResultException(const int line, const char* file, const HRESULT hresult) noexcept
	:
	Exception(line, file),
	hresult_(hresult)
{
}

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

std::string Window::Exception::translateErrorCode(const HRESULT result) noexcept
{
	std::string message = std::system_category().message(result);
	return message;
}

// Internal Window Class
Window::WindowClass::WindowClass()
	: instanceHandle_(GetModuleHandle(nullptr))
{
	PLOGV << "Instantiate Window Class";

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = handleMsgSetup;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instanceHandle_;
	wcex.hIcon = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = nullptr;
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = windowClassName;
	wcex.hIconSm = LoadIcon(instanceHandle_, MAKEINTRESOURCE(IDI_ICON1));

	RegisterClassEx(&wcex);
}

Window::WindowClass::~WindowClass()
{
	PLOGV << "Destroy Window Class";
	UnregisterClass(windowClassName, instanceHandle_);
}

LPCWSTR Window::WindowClass::getName() const noexcept
{
	return windowClassName;
}

HINSTANCE Window::WindowClass::getInstance() const noexcept
{
	return instanceHandle_;
}

// Message Handlers
LRESULT CALLBACK Window::handleMsgSetup(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) noexcept
{
#ifdef LOG_WINDOW_MESSAGES // defined in LoggingConfig.h
	PLOGV << windowsMessageMap(msg, lParam, wParam).c_str();
#endif

	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);  // NOLINT(performance-no-int-to-ptr)
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		// Store pointer in GWLP_USERDATA
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

		// Switch WndProc to thunk
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::handleMsgThunk));

		// Forward WM_NCCREATE to instance handler
		return pWnd->handleMsgImpl(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::handleMsgThunk(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) noexcept
{
#ifdef LOG_WINDOW_MESSAGES // defined in LoggingConfig.h
	PLOGV << windowsMessageMap(msg, lParam, wParam).c_str();
#endif
	// Retrieve Window* from GWLP_USERDATA
	Window* pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (!pWnd)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	// Forward to Window's own handler for now
	return pWnd->handleMsgImpl(hWnd, msg, wParam, lParam);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT Window::handleMsgImpl(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
#ifdef LOG_WINDOW_MESSAGES
	PLOGV << windowsMessageMap(msg, lParam, wParam).c_str();
#endif
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_CREATE:
		RECT rect;
		GetWindowRect(hWnd, &rect);
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
			return DefWindowProc(hWnd, msg, wParam, lParam);
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
			GetWindowRect(hWnd, &rect);
			setPosition(rect.left, rect.top);
		}
		break;
	case WM_MOVE:
	{
		const int x_pos = static_cast<short>(LOWORD(lParam)); // Horizontal position
		const int y_pos = static_cast<short>(HIWORD(lParam)); // Vertical position

		auto [x, y] = getPosition();
		const int x_delta = x_pos - x;
		const int y_delta = y_pos - y;

#ifdef LOG_WINDOW_MOVEMENT_MESSAGES
		PLOGD << "Window moved: " << x_delta << ", " << y_delta;
#endif

		// Update the mouse position so that the elements in the window which are dependent on the mouse position, are updated to appear stationary
		mouse_->onMouseMove(mouse_->pos().x - x_delta, mouse_->pos().y - y_delta);

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
		else if (!inSizeMove_)
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
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

