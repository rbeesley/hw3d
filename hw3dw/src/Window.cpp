#include "Window.hpp"

#include <system_error>

#include "App.hpp"
#include "AppConfig.hpp"
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
	, name_(name)
	, windowClass_(std::make_unique<WindowClass>()) {
	PLOGI << "Initialize Window";
}

Window::~Window()
{
	PLOGD << "Destroy Window";

	// Mark tearing down so handlers can avoid touching members
	isDestroying_ = true;

	// Hide and destroy the window. DestroyWindow may synchronously send messages.
	if (windowHandle_)
	{
		auto ctx = reinterpret_cast<WndContext*>(GetWindowLongPtr(windowHandle_, GWLP_USERDATA));

		ShowWindow(windowHandle_, SW_HIDE);
		DestroyWindow(windowHandle_);

		// After DestroyWindow returns, free the WndContext stored in GWLP_USERDATA
		// BUGBUG: This may be leaking
		if (ctx) {
			// clear GWLP_USERDATA to avoid dangling pointer
			SetWindowLongPtr(windowHandle_, GWLP_USERDATA, 0);

			// If we stored a thunk object pointer on the HWND via SetProp, retrieve and delete it
			HANDLE thunkHandle = GetProp(windowHandle_, L"AtumThunkObject");
			if (thunkHandle)
			{
				RemoveProp(windowHandle_, L"AtumThunkObject");

				using AppThunkType = lunaticpp::thunk<&App::thunkEntry>;
				AppThunkType* thunkObj = reinterpret_cast<AppThunkType*>(thunkHandle);
				try
				{
					delete thunkObj;
				}
				catch (...)
				{
					PLOGW << "Failed to delete thunk object during Window::~Window cleanup";
				}
			}

			delete ctx;
		}
		windowHandle_ = nullptr;
	}

	// Reset the mainWindowHandle
	App::setMainWindowHandle(nullptr);

	// Now release owned objects
	if (keyboard_) keyboard_.reset();
	if (mouse_) mouse_.reset();
	if (graphics_) graphics_.reset();
}

bool Window::ImGui::IsImGuiReady()
{
	return ::ImGui::GetCurrentContext() != nullptr;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT Window::ImGui::WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (!IsImGuiReady()) { return 1; }
#ifdef LOG_WINDOW_MESSAGES
	PLOGV << windowsMessageMap(msg, lParam, wParam).c_str();
#endif
#ifdef LOG_GRAPHICS_MESSAGES
	PLOGV << "ImGui_ImplWin32_WndProcHandler";
#endif
	return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}

// Message Handling
LRESULT Window::WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
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
		if (keyboard_) {
			keyboard_->clearState();
		}
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
		if (mouse_) {
			mouse_->onMouseMove(mouse_->pos().x - x_delta, mouse_->pos().y - y_delta);

			// Store the new position for future calculations
			setPosition(x_pos, y_pos);
		}
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
	case WM_MOUSEMOVE:
		// Dear ImGui, a member of Graphics, gets the first opportunity to handle Mouse messages
		if (graphics_ && graphics_->WndProcHandler(hWnd, msg, wParam, lParam))
		{
			return 0;
		}
		// If Dear ImGui didn't process the mouse message, Mouse processes it.
		if (mouse_)
		{
			const auto [x, y] = MAKEPOINTS(lParam);
			const auto [width, height] = getDimensions();
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
		if (ImGuiIO& io = ::ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
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

// Accessors
HWND Window::getHandle() const noexcept { return windowHandle_; }
Graphics& Window::getGraphics() const noexcept { return *graphics_.get(); }
Mouse& Window::getMouse() const noexcept { return *mouse_.get(); }
Keyboard& Window::getKeyboard() const noexcept { return *keyboard_.get(); }

// Window Management
void Window::create(void* createParams)
{
	// createWindow will call CreateWindowEx(..., createParams) which triggers WM_NCCREATE.
	// The setup thunk stores the createParams (expected to be WndContext*) into GWLP_USERDATA.
	auto* wndHnd = createWindow(createParams);
	App::setMainWindowHandle(wndHnd);

	// After CreateWindowEx returns, WM_NCCREATE should have run and GWLP_USERDATA contains the WndContext*
	if (auto ctx = reinterpret_cast<WndContext*>(GetWindowLongPtr(windowHandle_, GWLP_USERDATA))) {
		ctx->windowPtr = this;
	}

	graphics_ = std::make_unique<Graphics>(windowHandle_, width_, height_);

	// Check for an error
	if (!graphics_)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	if (targetWidth_ == width_ && targetHeight_ == height_)
	{
		setTargetDimensions(0, 0);
	}

	mouse_ = std::make_unique<Mouse>();
	keyboard_ = std::make_unique<Keyboard>();
}

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
	wcex.lpfnWndProc = WndProcHandlerSetup;
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

// Window Management
HWND Window::createWindow(void* createParams)
{
	// ReSharper disable once CppInitializedValueIsAlwaysRewritten
	RECT rect = { 100, 100, 100 + width_, 100 + height_ };

	if (!AdjustWindowRect(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	WndContext* const ctx = static_cast<WndContext*>(createParams);

	windowHandle_ = CreateWindowEx(
		0,
		windowClass_->getName(),
		name_,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		windowClass_->getInstance(),
		createParams
	);

	// Check for an error
	if (!windowHandle_)
	{
		throw ATUM_WND_LAST_EXCEPT();
	}

	ctx->windowPtr = windowHandle_;

	// Newly created windows start off as hidden
	PLOGI << "Show the Window";
	ShowWindow(windowHandle_, SW_SHOWDEFAULT);
	UpdateWindow(windowHandle_);

	return windowHandle_;
}

// Message Handlers
LRESULT CALLBACK Window::WndProcHandlerSetup(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam) noexcept
{
#ifdef LOG_WINDOW_MESSAGES // defined in LoggingConfig.h
	PLOGV << windowsMessageMap(msg, lParam, wParam).c_str();
#endif

	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);  // NOLINT(performance-no-int-to-ptr)
		// Expect lpCreateParams to be a WndContext*
		WndContext* const ctx = static_cast<WndContext*>(pCreate->lpCreateParams);

		// Store pointer in GWLP_USERDATA so other code can find it
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ctx));

		// Create a machine-code thunk that will call App::thunkEntry on the App instance.
		try
		{
			// instantiate a thunk that calls App::thunkEntry (non-noexcept wrapper)
			using AppThunkType = lunaticpp::thunk<&App::thunkEntry>;

			App* appPtr = reinterpret_cast<App*>(ctx->appPtr);

			// Allocate the thunk object on the heap and keep it as a HWND property
			AppThunkType* thunkObj = new AppThunkType(appPtr);

			// Get the generated function pointer (machine-code) to install as WNDPROC
			WNDPROC thunkProc = reinterpret_cast<WNDPROC>(thunkObj->func());

			// Store the thunk object pointer on the HWND so we can destroy it later.
			SetProp(hWnd, L"AtumThunkObject", reinterpret_cast<HANDLE>(thunkObj));

			// Install the thunk as the window procedure
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(thunkProc));

			// Forward WM_NCCREATE to the new thunk so it runs the same initialization path
			return CallWindowProc(thunkProc, hWnd, msg, wParam, lParam);
		}
		catch (...)
		{
			// If anything goes wrong, clean up and fall back to default behavior
			SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
			PLOGW << "Failed to create per-window thunk; falling back to DefWindowProc";
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
